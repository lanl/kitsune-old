//===- GatherScatter.cpp - gather-scatter opt                     ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/Verifier.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"

#include "llvm/Analysis/LoopInfo.h"

#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"

using namespace llvm;
// using namespace llvm::orc;

namespace {

  std::string idx_var;

  struct GatherScatter : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    GatherScatter() : ModulePass(ID) {}

    void add_while(Function* f, Module &M, BasicBlock* lp_body){
      IRBuilder<> Builder(M.getContext());

      Function::arg_iterator args = f->arg_begin();
      Value* vtrack = args++;
      Value* vloc = args++;

      Function *cF = f; //Builder.GetInsertBlock()->getParent(); // Could prob just set it to it, but hey

        Builder.SetInsertPoint(&(cF->getEntryBlock()));
        
        BasicBlock *PreBB = Builder.GetInsertBlock(); //Builder.GetInsertBlock();

        BasicBlock *LoopCBB = BasicBlock::Create(M.getContext(), "whilelpcond", cF);
        Instruction *split = PreBB->getFirstNonPHI(); 
        //Instruction *split = PreBB->getTerminator(); 
        //llvm::errs() << "split : " << *split << "\n";
        BasicBlock *head = split->getParent();
        BasicBlock *tail = head->splitBasicBlock(split, "while.body"); 
        head->getTerminator()->eraseFromParent();
        Builder.SetInsertPoint(head);

        AllocaInst *track = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "track");
        Builder.CreateStore(vtrack, track);
        
        AllocaInst *loc = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "loc");
        Builder.CreateStore(vloc, loc);
        


        AllocaInst *Alloca = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "tracker");
        Builder.CreateStore(vtrack, Alloca);

        Builder.CreateBr(LoopCBB);
        // I->eraseFromParent();
        // Not sure if it is important, but tail's pred is only loopcmp, and no longer includes ref to main 


        Builder.SetInsertPoint(LoopCBB);
        BasicBlock *LoopBB = BasicBlock::Create(M.getContext(), "whilelpend", cF);

        
        // BasicBlock *PostLoopBB = BasicBlock::Create(M.getContext(), "PostLoopBB", main);

        Value *trkr = cF->getValueSymbolTable()->lookup("tracker");

        Value *addition = Builder.CreateNSWAdd(vtrack,
            Builder.CreateLoad(M.getNamedValue("buffer_size"), "buffer_size"));

        Value *endCond = Builder.CreateICmpSLT(Builder.CreateLoad(trkr, "trkr"), addition);

        // PUT BACK?? Builder.CreateCondBr(endCond, tail, LoopBB); // TODO: fix where to go 
        
        
        BasicBlock *land = BasicBlock::Create(M.getContext(), "land.rhs", cF, tail);
        BasicBlock *landend = BasicBlock::Create(M.getContext(), "land.end", cF, tail);
        
        Builder.CreateCondBr(endCond, land, landend);

        Builder.SetInsertPoint(land);
        LoadInst *load_trkr = Builder.CreateLoad(trkr, "trkr");
        LoadInst *load_lst = Builder.CreateLoad(M.getNamedValue("list_size"), "lst_size");
        Value *sub = Builder.CreateNSWSub(load_lst, ConstantInt::get(Type::getInt32Ty(M.getContext()), 1), "sub");
        Value *cmp2 = Builder.CreateICmpSLT(load_trkr, sub); 
        Builder.CreateBr(landend);

        Builder.SetInsertPoint(landend);
        PHINode *PN = Builder.CreatePHI(Type::getInt1Ty(M.getContext()), 2, "pn");
        PN->addIncoming(ConstantInt::get(Type::getInt1Ty(M.getContext()), 0), LoopCBB);
        PN->addIncoming(cmp2, land);
        Builder.CreateCondBr(PN, tail, LoopBB);

        //  
        Builder.SetInsertPoint(LoopBB);
        Builder.CreateBr(LoopCBB);
        
        for (inst_iterator I = inst_begin(cF), E = inst_end(cF); I != E; ++I){
          if (I->getOpcode() == Instruction::Ret){
            
            BasicBlock *end_block = I->getParent();
           
            Instruction *new_I = &*I;
            Instruction *ret_void = new_I->clone();
            ++I;

           Instruction *whileEnd = LoopBB->getTerminator();
           Instruction *br_while_cond = whileEnd->clone();

           br_while_cond->insertBefore(new_I);
           new_I->eraseFromParent();

           ret_void->insertBefore(whileEnd);
           whileEnd->eraseFromParent();
            break;
          }
        }

        for (inst_iterator I = inst_begin(cF), E = inst_end(cF); I != E; ++I){

          if (I->getOpcode() == Instruction::ICmp){
            Value *N = I->getOperand(1);
            ConstantInt *lstsz = dyn_cast<llvm::ConstantInt>(N);
            if (!lstsz){
              continue;
            }
            else{
              if (lstsz->equalsInt(100)){
                Builder.SetInsertPoint(&*I);
                Instruction *ld_bsize = Builder.CreateLoad(M.getNamedValue("buffer_size"));
                I->setOperand(1, ld_bsize);
              }
            }
          }

        // if gather and A, replace A[idx[i]] with A[idx[tracker]]
        // if compute, leave g_A[i] and s_A[i]
        // if scatter and A, replace A[idx[i]] with A[idx[tracker]]
          if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP
            if (I->getOperand(0)->getName() == idx_var){
              Builder.SetInsertPoint(&*I);
              Value *t_load = Builder.CreateLoad(trkr, "trkr");
              I->setOperand(2, t_load);
            }
          } 
        }



      // BasicBlock *lp_body = GEPg_A->getParent();
      Instruction *lp_bod_term = lp_body->getTerminator();
      Builder.SetInsertPoint(lp_bod_term);

      Builder.CreateStore(Builder.CreateNSWAdd(Builder.CreateLoad(trkr), 
        ConstantInt::get(Type::getInt32Ty(M.getContext()), 1)),
        trkr);
    }

    bool runOnModule(Module &M) override {
      errs() << "running on module\n";

      std::string gather_var = "";
      idx_var = "";
      unsigned bsize = 1;
      unsigned lsize = 1;

       MDNode *meta_data; 
       bool PragmaEnablePv = false;
       Function *compute_function;


      for (auto F = M.getFunctionList().begin(), 
        eF = M.getFunctionList().end(); 
        F != eF; ++F) {

        if ((*F).size() == 0) continue;
        LoopInfo &li = getAnalysis<LoopInfoWrapperPass>(*F).getLoopInfo();

        for (Function::iterator b = F->begin(), be = F->end(); b != be; ++b) {
          BasicBlock *BB = &*b;
          Loop *lp = li.getLoopFor(BB);

          if (lp){
                // errs() << *lp << '\n';
            MDNode *LoopID = lp->getLoopID();
            if (LoopID){
              MDNode *MD = dyn_cast<MDNode>(LoopID->getOperand(1));
              if (MD->getNumOperands() == 5){
                MDString *S = dyn_cast<MDString>(MD->getOperand(1));
                if (S){
                  gather_var = S->getString();
                }
                MDString *S2 = dyn_cast<MDString>(MD->getOperand(2));
                if (S2){
                  idx_var = S2->getString();
                }

                bsize = mdconst::extract<ConstantInt>(MD->getOperand(3))->getZExtValue();
                lsize = mdconst::extract<ConstantInt>(MD->getOperand(4))->getZExtValue();
              }

              meta_data = GetUnrollMetadata(LoopID, "llvm.loop.pv.enable");
              if (meta_data){
                PragmaEnablePv = true;
                compute_function = BB->getParent();
              }
            }
          } // end if(lp)
        } // end funct iter
      } // end func


//// Create the buffers

      GlobalVariable* gA_buffs = new GlobalVariable(M, ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2),
          false, GlobalValue::CommonLinkage, 0, "gAbuffs");
     
      gA_buffs->setAlignment(16); 
      ConstantAggregateZero* const_array_2 = ConstantAggregateZero::get(ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2));

      gA_buffs->setInitializer(const_array_2);


      GlobalVariable* buff_q = new GlobalVariable(M, PointerType::get(Type::getDoublePtrTy(M.getContext()), 0),
          false, GlobalValue::ExternalLinkage, 0, "buff_q"); // common or gloabal linkage here? 
     
      buff_q->setAlignment(8); 

      std::vector<Value*> index_vector;
      index_vector.push_back((Value*)ConstantInt::get(Type::getInt32Ty(M.getContext()), 0));
      index_vector.push_back((Value*)ConstantInt::get(Type::getInt32Ty(M.getContext()), 0));

      Constant *gep_const = ConstantExpr::getGetElementPtr(
          ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2), 
          gA_buffs,
         index_vector);

      buff_q->setInitializer(gep_const);
      

      GlobalVariable* sA_buffs = new GlobalVariable(M, ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2),
          false, GlobalValue::CommonLinkage, 0, "sAbuffs");
     
      sA_buffs->setAlignment(16); 
      ConstantAggregateZero* sconst_array_2 = ConstantAggregateZero::get(ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2));

      sA_buffs->setInitializer(sconst_array_2);


      GlobalVariable* sbuff_q = new GlobalVariable(M, PointerType::get(Type::getDoublePtrTy(M.getContext()), 0),
          false, GlobalValue::ExternalLinkage, 0, "sbuff_q"); // common or gloabal linkage here? 
     
      sbuff_q->setAlignment(8); 

      std::vector<Value*> sindex_vector;
      sindex_vector.push_back((Value*)ConstantInt::get(Type::getInt32Ty(M.getContext()), 0));
      sindex_vector.push_back((Value*)ConstantInt::get(Type::getInt32Ty(M.getContext()), 0));

      Constant *sgep_const = ConstantExpr::getGetElementPtr(
          ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2), 
          sA_buffs,
         sindex_vector);

      sbuff_q->setInitializer(sgep_const);
//// END Create the buffers




      // Make a gather variable for A 
      GlobalVariable* g_A;
      GlobalVariable* s_A;
      GlobalVariable* A;
      GlobalVariable* buffer_size;
      GlobalVariable* main_tracker;

      buffer_size = new GlobalVariable(M, Type::getInt32Ty(M.getContext()), false, GlobalVariable::ExternalLinkage,
        ConstantInt::get(Type::getInt32Ty(M.getContext()), bsize), "buffer_size");
      main_tracker = new GlobalVariable(M, Type::getInt32Ty(M.getContext()), false, GlobalVariable::ExternalLinkage,
        ConstantInt::get(Type::getInt32Ty(M.getContext()), lsize), "main_tracker");

      // PUT THIS BACK???
      for (auto &globalVar : M.getGlobalList()){
        if (globalVar.getName() == gather_var){ 
          A = &globalVar;
          // TODO: fix the initializers so that the gather and scatter buffs are buffer_size, not list_size. 
          g_A = new GlobalVariable(M, globalVar.getValueType(), globalVar.isConstant(), globalVar.getLinkage(),
            globalVar.getInitializer(), "g_" + globalVar.getName(), 
            (GlobalVariable*) nullptr, globalVar.getThreadLocalMode(), 
            globalVar.getType()->getAddressSpace());
          s_A = new GlobalVariable(M, globalVar.getValueType(), globalVar.isConstant(), globalVar.getLinkage(),
            globalVar.getInitializer(), "s_" + globalVar.getName(), 
            (GlobalVariable*) nullptr, globalVar.getThreadLocalMode(), 
            globalVar.getType()->getAddressSpace());
          continue;
        }
      }

      llvm::ValueToValueMapTy vmap;
      Function *gatherF;
      Function *computeF;
      Function *scatterF;
      llvm::ValueToValueMapTy s_vmap; // necessary to have 2?

      IRBuilder<> Builder(M.getContext());

      std::vector<Type*> new_args(2, Type::getInt32Ty(M.getContext()));
      
      FunctionType *FT = FunctionType::get(Type::getVoidTy(M.getContext()),
                                                   new_args, false);


      Function *compute_copy = Function::Create(FT, Function::ExternalLinkage, "new_comput", &M);

      llvm::ValueToValueMapTy vmap3;
      llvm::SmallVector<llvm::ReturnInst*, 2> Returns;
      CloneFunctionInto(compute_copy, compute_function, vmap3, 0, Returns);
      Function::arg_iterator args = compute_copy->arg_begin();
      Value* x = args++;
      x->setName("track");
      Value* y = args++;
      y->setName("loc");
//      llvm::SmallVector<Value*, 2> zero_args;
//      Value* v = dyn_cast<Value>(new LoadInst(M.getNamedValue("buffer_size"))); 
//      zero_args.push_back(v);
//      v = dyn_cast<Value>(new LoadInst(M.getNamedValue("buffer_size"))); 
//      zero_args.push_back(v);
      //zero_args.push_back(dyn_cast<Value*>(ConstantInt::get(Type::getInt32Ty(M.getContext()), 10)));
      //zero_args.push_back(ConstantInt::get(Type::getInt32Ty(M.getContext()), 10));
    Instruction *test_c_call;

      Instruction *compute_call_to_delete;
      for (User *U : compute_function->users()) {
        if (Instruction *Inst = dyn_cast<Instruction>(U)) {
        //int i = 0;
        for (Use &U2 : Inst->operands()) {
            Value *v = U2.get();
            if (v == compute_function){
              llvm::errs() << "found compute\n";

              std::vector<Value*> args1;

              args1.push_back(dyn_cast<Value>(new LoadInst(M.getNamedValue("buffer_size"), "load buff 1", Inst)));
              args1.push_back(dyn_cast<Value>(new LoadInst(M.getNamedValue("buffer_size"), "load buff 2", Inst)));
 //     llvm::SmallVector<Value*, 2> zero_args;
 //     Value* v = dyn_cast<Value>(new LoadInst(M.getNamedValue("buffer_size"), "load buff 1", Inst)); 
 //     zero_args.push_back(v);
 //     v = dyn_cast<Value>(new LoadInst(M.getNamedValue("buffer_size"), "load buff 2", Inst)); 
 //     zero_args.push_back(v);
                //llvm::errs() << "v == compute_function\n";
                
               //Builder.SetInsertPoint(Inst->getNextNode());
               Builder.SetInsertPoint(Inst);
               // Builder.CreateCall(M.getFunction("new_comput"), ArrayRef<Value*>(args1));
               test_c_call = Builder.CreateCall(compute_copy, ArrayRef<Value*>(args1));
               //llvm::errs() << FT->getNumParams() << "\n"; 
               llvm::errs() << *Inst << "\n";
               compute_call_to_delete = Inst;
                // Inst->removeFromParent();
                //Inst->setOperand(i, compute_copy);
                //Inst->setOperand();
               //v = compute_copy; 
            }
            //++i;
          }        
        }
      }
      compute_call_to_delete->eraseFromParent();
     llvm::errs() << "got this far!\n"; 
      //compute_function->replaceAllUsesWith(compute_copy);
      //compute_function->removeFromParent();
      // Obviously this is not the best way to do this.







      /// buffer init

      Builder.SetInsertPoint(test_c_call);
      FunctionType *ib_ft ;
      Constant *init_buff;
      Function *ib_func;
      Instruction *call ;
      Instruction *ld_bq;
      ConstantInt *ld_i;
      Value *array_idx ;

    for (int i = 0; i < 2; ++i){
      ib_ft = FunctionType::get(Type::getDoublePtrTy(M.getContext()), false);
      init_buff = M.getOrInsertFunction("init_double_buffer", ib_ft);
      ib_func = dyn_cast<Function>(init_buff);
      
      call = Builder.CreateCall(ib_func, llvm::NoneType::None, "call");
      ld_bq = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
      ld_i = ConstantInt::get(Type::getInt32Ty(M.getContext()), i);
      array_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), ld_bq, ld_i, "arrayidx");
      Builder.CreateStore(call, array_idx, false);
      
      call = Builder.CreateCall(ib_func, llvm::NoneType::None, "call");
      ld_bq = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "ld_sbq" );
      array_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), ld_bq, ld_i, "arrayidx");
      Builder.CreateStore(call, array_idx, false);
    }

      /// END buffer init




      gatherF = CloneFunction(compute_copy, vmap);
      //gatherF = CloneFunction(compute_function, vmap);
      gatherF->setName("gather");

      compute_function->eraseFromParent();

      Instruction *GEPg_A; //GEP for g_A[i] = ... 
      Instruction *ld_idx_A; // ... = A[idx[i]];
      Value *lp_i; // i in g_A[i]

      Instruction *old_inst;

      for (inst_iterator I = inst_begin(gatherF), E = inst_end(gatherF); I != E; ++I){


        if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP

          if (I->getOperand(0)->getName() == idx_var){
            lp_i = I->getOperand(2);
          }

          if (I->getOperand(0)->getName() == gather_var){
            old_inst = &*I;
            GEPg_A = I->clone(); // Cloning GEP for A because we need to add a GEP for g_A
            // A[idx[i]] => g_A[i]
            // IMPORTANT: it shouild be g_A[tracker], not g_A[i]
            // But need to preserve A[idx[i]] because we will store that val in g_A[i].

            //TODO
            // GEPg_A->setOperand(2, Builder.CreateLoad(M.getNamedValue("tracker"))); // Fixing operand from idx[i] to just i

//            for (User *U : I->users()) {
//              if (Instruction *Inst = dyn_cast<Instruction>(U)) {
//                if (isa<LoadInst>(Inst)) {
//                  // grab the load for the operand for g_A's store. 
//                  ld_idx_A = Inst;
//
//                  // delete everything that uses the load
//                  for (User *LU : ld_idx_A->users()) {
//                    if (Instruction *LInst = dyn_cast<Instruction>(LU)) {
//                      LInst->eraseFromParent();
//                    }
//                  }
//                }
//                // delete stores to A
//                else if (isa<StoreInst>(Inst)){
//                  Inst->eraseFromParent();
//                }
//              }
//            }
//
//            // TODO: use the builder to insert instructions instead!!!! 
//            GEPg_A->insertAfter(ld_idx_A); // insert after load
//
//
//            // Instruction *store_A = new StoreInst(ld_idx_A, GEPg_A, GEPg_A->getParent()->getTerminator());
//            Instruction *store_A = new StoreInst(ld_idx_A, GEPg_A);
//            store_A->insertBefore(GEPg_A->getParent()->getTerminator());


            //add_while(gatherF, M, GEPg_A->getParent());
   

        }

      } 

    } // end instruction iterator on gatherF

      Instruction *gld_buffer_q;
      Value *gq_location;
      Value *grem ;
      Value *gbuff_idx;
      Instruction *ld_gbf_idx;

      Function::arg_iterator gargs = gatherF->arg_begin();
      Value* track_arg = gargs++;
      //x->setName("track");
      Value* loc_arg = gargs++;
      //y->setName("loc");


      GEPg_A->setOperand(0, g_A); // Fixing operand from A to g_A 
      //GEPg_A->setOperand(0, ld_gbf_idx); // Fixing operand from A to g_A 



      GEPg_A->setOperand(2, lp_i); // Fixing operand from idx[i] to just i
     // TODO: make this better and fix it to use the new buffers! 
            for (User *U : old_inst->users()) {
              if (Instruction *Inst = dyn_cast<Instruction>(U)) {
                if (isa<LoadInst>(Inst)) {
                  // grab the load for the operand for g_A's store. 
                  ld_idx_A = Inst;

                  // delete everything that uses the load
                  for (User *LU : ld_idx_A->users()) {
                    if (Instruction *LInst = dyn_cast<Instruction>(LU)) {
                      LInst->eraseFromParent();
                    }
                  }
                }
                // delete stores to A
                else if (isa<StoreInst>(Inst)){
                  Inst->eraseFromParent();
                }
              }
            }

            // TODO: use the builder to insert instructions instead!!!! 
            GEPg_A->insertAfter(ld_idx_A); // insert after load


            
      
      /// April 4
      //Builder.SetInsertPoint(GEPg_A); 
     // Builder.SetInsertPoint(old_inst); 
      Builder.SetInsertPoint(ld_idx_A->getNextNode());
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
      //gq_location = Builder.CreateLoad(Type::getInt32Ty(M.getContext()), gatherF->getValueSymbolTable()->lookup("track"), "q_loc");
      //gq_location = Builder.CreateLoad(Type::getInt32Ty(M.getContext()), gatherF->getValueSymbolTable()->lookup("loc2"), "q_loc");
      grem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
      //grem = Builder.CreateSRem(gq_location, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, grem, "buff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "load_buff_idx"); 

      Value *gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, lp_i, "gep_at_bf");
      Instruction *str_A_gbf = Builder.CreateStore(ld_idx_A, gep_at_bf, "store_A_new_buff");
// This str_A_bf creates an issue later when deleting instructions from scatter method. 
            
            // Instruction *store_A = new StoreInst(ld_idx_A, GEPg_A, GEPg_A->getParent()->getTerminator());
//to delete
        //    Instruction *store_A = new StoreInst(ld_idx_A, GEPg_A);
         //   store_A->insertBefore(GEPg_A->getParent()->getTerminator());
//end to delete

      add_while(gatherF, M, GEPg_A->getParent());

// okay so far




      scatterF = CloneFunction(compute_copy, s_vmap);
      scatterF->setName("scatter");
      Instruction *gep_A_idx;
      Instruction *gep_gA_i;
      Instruction *ld_gA_i;
      // Instruction *str_A_idx;
      std::vector<Instruction*> s_discards;


      // Instruction *s_GEPg_A; //GEP for g_A[i] = ... 
      // Value *s_A;
/*
      for (inst_iterator I = inst_begin(scatterF), E = inst_end(gatherF); I != E; ++I){


        if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP

          if (I->getOperand(0)->getName() == idx_var){
            lp_i = I->getOperand(2);
          }

          if (I->getOperand(0)->getName() == gather_var){
            old_inst = &*I;
            GEPg_A = I->clone(); // 
}}}
*/


Instruction *gep_idx_var; // gep for idx var
Instruction *gep_gather_var; // gep for A var

      for (inst_iterator I = inst_begin(scatterF), E = inst_end(scatterF); I != E; ++I){

              if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP

                if (I->getOperand(0)->getName() == gather_var){
                    gep_gather_var = &*I; 
                }
                if (I->getOperand(0)->getName() == idx_var){
                    gep_idx_var = &*I; 
                }
            
            } // end if GEP 
          } // end inst_iterator I = inst_begin(scatterF)

            for (User *U : gep_gather_var->users()) {
              if (Instruction *Inst = dyn_cast<Instruction>(U)) {
                if (isa<LoadInst>(Inst)) {
                  // grab the load for the operand for g_A's store. 
                  ld_idx_A = Inst;

                  // delete everything that uses the load
                  for (User *LU : ld_idx_A->users()) {
                    if (Instruction *LInst = dyn_cast<Instruction>(LU)) {
                      LInst->eraseFromParent();
                    }
                  }
                }
                // delete stores to A
                else if (isa<StoreInst>(Inst)){
                  Inst->eraseFromParent();
                }
              }
            }
          ld_idx_A->eraseFromParent();


      Function::arg_iterator sargs = scatterF->arg_begin();
      track_arg = sargs++;
      loc_arg = sargs++;


      Builder.SetInsertPoint(gep_idx_var);
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "sld_bq" );
      grem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "srem"); 
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, grem, "sbuff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "sload_buff_idx"); 
      
      gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, gep_idx_var->getOperand(2), "sgep_at_bf");
      Value *sload = Builder.CreateLoad(Type::getDoubleTy(M.getContext()), gep_at_bf, "sld" );

      //gep_idx_var->setOperand(2, track_arg);
      
      
      Builder.SetInsertPoint(gep_idx_var->getParent()->getTerminator());
      //str_A_gbf = Builder.CreateStore(ld_idx_A, gep_at_bf, "sstore_A_new_buff");
      str_A_gbf = Builder.CreateStore(sload, gep_gather_var, "sstore_A_new_buff");



      add_while(scatterF, M, gep_idx_var->getParent());






      // TODO MARCH 27:
      // Need to fix the compute function to use the new buffers!!!!

          // Instruction *GEPg_A2; //GEP for g_A[i] = ... 
          Value *lp_i2; // i in g_A[i]
          // Instruction *idx_instr;
          computeF = compute_copy;
          // computeF = compute_function;

          Instruction *str_in_s_A;
          Value *val_for_store;
          std::vector<Instruction*> to_delete; 

      Instruction *ld_buffer_q;
      Value *q_location;
      Value *rem ;
      Value *buff_idx;
      
      Function::arg_iterator cargs = computeF->arg_begin();
      track_arg = cargs++;
      loc_arg = cargs++;

      Instruction *idx_gep_inst;
      Instruction *gat_gep_inst;
      Instruction *store_res;

      Instruction *sld_buffer_q;
      Value *sbuff_idx;
      Value *gep_at_sbf;
      Instruction *ld_sbf_idx;
      Value *srem;
      Instruction *ld_frm_A;
      Instruction *icmp;

          for (inst_iterator I = inst_begin(computeF), E = inst_end(computeF); I != E; ++I){
    //        // Okay now replace A[idx[i]] += 1; to be g_A[i] += 1;
    //        // Now this needs to be s_A[i] = g_A[i] + 1; 

            if (I->getOpcode() == Instruction::GetElementPtr){
              if (I->getOperand(0)->getName() == idx_var){ 
                llvm::errs() << "found gep idx\n";
                idx_gep_inst = &*I;
              }
              if (I->getOperand(0)->getName() == gather_var){ 
                llvm::errs() << "found gep A\n";
                gat_gep_inst = &*I; 
              }
            } 

            if (isa<StoreInst>(dyn_cast<Instruction>(&*I))){
              if(I->getOperand(1) == gat_gep_inst){
                llvm::errs() << "found store to A\n";
                store_res = &*I;
              }
            }
            if (isa<LoadInst>(dyn_cast<Instruction>(&*I))){
              if(I->getOperand(0) == gat_gep_inst){
                llvm::errs() << "found load from A\n";
                ld_frm_A = &*I;
              }
            }
            if (isa<ICmpInst>(dyn_cast<Instruction>(&*I))){
                llvm::errs() << "found icmp\n";
                icmp = &*I;
            }
          }

      Builder.SetInsertPoint(&*(inst_begin(computeF)));
      Value *ld_bsize_icmp = Builder.CreateLoad(Type::getInt32Ty(M.getContext()), M.getNamedValue("buffer_size"), "icmp_bsize");

      icmp->setOperand(1, ld_bsize_icmp);      

      Builder.SetInsertPoint(idx_gep_inst); 
     rem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 

 // gather stuff
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, rem, "buff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "load_buff_idx"); 
      gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, idx_gep_inst->getOperand(2), "gep_at_bf");
      //str_A_gbf = Builder.CreateStore(ld_idx_A, gep_at_bf, "store_A_new_buff");
      ld_frm_A->setOperand(0, gep_at_bf);
            
//scatter stuff
      sld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "sld_bq" );
      sbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), sld_buffer_q, rem, "sbuff_idx"); 
      ld_sbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), sbuff_idx, "sload_buff_idx"); 
      
      gep_at_sbf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()), ld_sbf_idx, idx_gep_inst->getOperand(2), "sgep_at_bf");
      sload = Builder.CreateLoad(Type::getDoubleTy(M.getContext()), gep_at_sbf, "sld" );
      //Instruction *store_test = Builder.CreateStore(ConstantFP::get(Type::getDoubleTy(M.getContext()), 50.0));

     

      //llvm::errs() << store_res->getOperand(0) << "\n";
      //llvm::errs() << store_res->getOperand(1) << "\n";

      //store_res->setOperand(1, );
      //Instruction *new_store_inst = store_res->clone();
      //new_store_inst->insertAfter(store_res);
      //new_store_inst->setOperand(1, gep_at_sbf);
      store_res->setOperand(1, gep_at_sbf);


    //         if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP, may be better way to do this. 

    //            if (I->getOperand(0)->getName() == idx_var){// this is hard coded right now, but can be generalized.

    //              lp_i2 = I->getOperand(2); // get third operand, which is i
    // 
    //             idx_inst = &*I; 
    //              // Delete idx instructions
    //              for (User *U : I->users()) {
    //                if (Instruction *Inst = dyn_cast<Instruction>(U)) {
    //                  if (isa<LoadInst>(Inst)) {
    //                    // delete everything that uses idx
    //                    for (User *LU : Inst->users()) {
    //                      if (Instruction *LInst = dyn_cast<Instruction>(LU)) {
    //                        to_delete.push_back(LInst);
    //                      }
    //                    }
    //                  }
    //                  // Delete idx instruction
    //                  to_delete.push_back(Inst);

    //                }
    //              }
    //              /////// NEED A BETTER SOLUTION to remove the GEP using idx.               
    //               Instruction *ins = &*I;
    //               ++I;
    //               to_delete.push_back(ins);

    //            }

    //            if (I->getOperand(0)->getName() == gather_var){
    //              str_in_s_A = I->clone(); // Use this to gep s_A, 
    //              str_in_s_A->setOperand(0, s_A);
    //              str_in_s_A->setOperand(2, lp_i2);

    //              I->setOperand(0, g_A); // Will not use A directly in compute. 
    //              I->setOperand(2, lp_i2);
    //              val_for_store = &(*I); // We will store the computed result in s_A
    //            }

    //        }

    //        // if (isa<StoreInst>(&(*I))){
    //        if (I->getOpcode() == Instruction::Store){
    //          if(I->getOperand(1) == val_for_store){
    //            str_in_s_A->insertBefore(&(*I));
    //            I->setOperand(1, str_in_s_A);
    //          }
    //        }


    //        if (I->getOpcode() == Instruction::ICmp){
    //          Value *N = I->getOperand(1);
    //          ConstantInt *lstsz = dyn_cast<llvm::ConstantInt>(N);
    //          if (!lstsz){
    //                continue;
    //              }
    //              else{
    //              if (lstsz->equalsInt(100)){
    //                Builder.SetInsertPoint(&*I);
    //                Instruction *ld_bsize = Builder.CreateLoad(M.getNamedValue("buffer_size"));
    //                I->setOperand(1, ld_bsize);
    //                }
    //              }
    //          
    //        }

    //      for (Instruction *inst : to_delete){
    //        inst->eraseFromParent();
    //      }






// removing things to refactor
/*
             if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP, may be better way to do this. 

                if (I->getOperand(0)->getName() == "idx"){// this is hard coded right now, but can be generalized.

                  lp_i2 = I->getOperand(2); // get third operand, which is i
     
                 idx_inst = &*I; 
                  // Delete idx instructions
                  for (User *U : I->users()) {
                    if (Instruction *Inst = dyn_cast<Instruction>(U)) {
                      if (isa<LoadInst>(Inst)) {
                        // delete everything that uses idx
                        for (User *LU : Inst->users()) {
                          if (Instruction *LInst = dyn_cast<Instruction>(LU)) {
                            to_delete.push_back(LInst);
                          }
                        }
                      }
                      // Delete idx instruction
                      to_delete.push_back(Inst);

                    }
                  }
                  /////// NEED A BETTER SOLUTION to remove the GEP using idx.               
                   Instruction *ins = &*I;
                   ++I;
                   to_delete.push_back(ins);

                }

                if (I->getOperand(0)->getName() == gather_var){
                  str_in_s_A = I->clone(); // Use this to gep s_A, 
                  str_in_s_A->setOperand(0, s_A);
                  str_in_s_A->setOperand(2, lp_i2);

                  I->setOperand(0, g_A); // Will not use A directly in compute. 
                  I->setOperand(2, lp_i2);
                  val_for_store = &(*I); // We will store the computed result in s_A
                }

            }

            // if (isa<StoreInst>(&(*I))){
            if (I->getOpcode() == Instruction::Store){
              if(I->getOperand(1) == val_for_store){
                str_in_s_A->insertBefore(&(*I));
                I->setOperand(1, str_in_s_A);
              }
            }


            if (I->getOpcode() == Instruction::ICmp){
              Value *N = I->getOperand(1);
              ConstantInt *lstsz = dyn_cast<llvm::ConstantInt>(N);
              if (!lstsz){
                    continue;
                  }
                  else{
                  if (lstsz->equalsInt(100)){
                    Builder.SetInsertPoint(&*I);
                    Instruction *ld_bsize = Builder.CreateLoad(M.getNamedValue("buffer_size"));
                    I->setOperand(1, ld_bsize);
                    }
                  }
              
            }
          }

          for (Instruction *inst : to_delete){
            inst->eraseFromParent();
          }

          add_while(compute_copy, M, str_in_s_A->getParent());
  */
          //add_while(compute_copy, M, str_in_s_A->getParent());

// removing things to refactor


         // add_while(compute_function, M, str_in_s_A->getParent());
    
          //Function::iterator b = computeF->begin() 
      // scatter stuff
      /*
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "sld_bq" );
      grem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "srem"); 
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, grem, "sbuff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "sload_buff_idx"); 
      
      gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, gep_idx_var->getOperand(2), "sgep_at_bf");
      Value *sload = Builder.CreateLoad(Type::getDoubleTy(M.getContext()), gep_at_bf, "sld" );
      
      Builder.SetInsertPoint(gep_idx_var->getParent()->getTerminator());
      str_A_gbf = Builder.CreateStore(sload, gep_gather_var, "sstore_A_new_buff");
      */


          // April 3 WiP
//      Builder.SetInsertPoint(str_in_s_A->getParent()->getFirstNonPHI()); 
//      ld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
//      q_location = Builder.CreateLoad(Type::getInt32Ty(M.getContext()), computeF->getValueSymbolTable()->lookup("track1"), "q_loc");
//      rem = Builder.CreateSRem(q_location, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
//      buff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), ld_buffer_q, rem, "buff_idx");
        // end April 3 WiP




// OLD SCATTER LOGIC
/*
      scatterF = CloneFunction(gatherF, s_vmap);
      scatterF->setName("scatter");
      Instruction *gep_A_idx;
      Instruction *gep_gA_i;
      Instruction *ld_gA_i;
      // Instruction *str_A_idx;
      std::vector<Instruction*> s_discards;


      // Instruction *s_GEPg_A; //GEP for g_A[i] = ... 
      // Value *s_A;

      for (inst_iterator I = inst_begin(scatterF), E = inst_end(scatterF); I != E; ++I){

              if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP

                  if (I->getOperand(0)->getName() == gather_var){
                    gep_A_idx = &*I;

                    // Delete the load of this value
                    for (User *U : I->users()) {
                      if (Instruction *Inst = dyn_cast<Instruction>(U)) {
                        if (isa<LoadInst>(Inst)) {
                          s_discards.push_back(Inst);
                        }
                      }
                    }
                  }

                  if (I->getOperand(0)->getName() == "g_A"){

                    gep_gA_i = &*I;
                    gep_gA_i->setOperand(0, M.getNamedValue("s_A")); //maybe this will work 
                    ld_gA_i = new LoadInst(gep_gA_i, "load_sA", gep_gA_i->getNextNode());

                    for (User *U : I->users()) {
                      if (Instruction *Inst = dyn_cast<Instruction>(U)) {
                        if (isa<StoreInst>(Inst)) {

                          Inst->setOperand(0, ld_gA_i);
                          Inst->setOperand(1, gep_A_idx);
                          
                        }
                      }
                    }
                }
            } // end if GEP 
          } // end inst_iterator I = inst_begin(scatterF)

          for (Instruction *d : s_discards){
            d->eraseFromParent();
          }

          // errs() << "after scatter" << *scatterF;
*/
// END OLD SCATTER LOGIC







          verifyFunction(*computeF);
          verifyFunction(*gatherF);
          verifyFunction(*scatterF);
          verifyModule(M);

          Instruction *synctoken_g0;
          Instruction *synctoken_g1;
          Instruction *synctoken_s0;
          Instruction *synctoken_s1;

         AllocaInst *Alloca_ct; 

          for (Module::iterator it = M.begin(), end = M.end(); it != end; ++it){
            if (it->getName().find("main") != std::string::npos){
              for (inst_iterator I = inst_begin(*it), E = inst_end(*it); I != E; ++I){

                // Insert assignment for buffer size
                if (isa<StoreInst>(&(*I))) {
                  if (I->getOperand(1) == M.getNamedValue("list_size")) {
                    // errs() << "found store to list size" << '\n';

                      llvm::errs() << "creating sync token\n";
                      // SYNC TOKEN! 
                      Builder.SetInsertPoint(&*I);
                      Function *synctok = Intrinsic::getDeclaration(&M, Intrinsic::syncregion_start);
                      synctoken_g0 = Builder.CreateCall(synctok, None, "synctoken_g0");
                      synctoken_g1 = Builder.CreateCall(synctok, None, "synctoken_g1");
                      synctoken_s0 = Builder.CreateCall(synctok, None, "synctoken_s0");
                      synctoken_s1 = Builder.CreateCall(synctok, None, "synctoken_s1");
                      
                      Alloca_ct = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "compute_tracker");
                      StoreInst *Store_ct = Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), Alloca_ct);                      


                    Instruction *str_buff_size = I->clone();
                    Value *buffer_size = ConstantInt::get(Type::getInt32Ty(M.getContext()), 10);
                    str_buff_size->setOperand(0, buffer_size);
                    str_buff_size->setOperand(1, M.getNamedValue("buffer_size"));
                    str_buff_size->insertAfter(&(*I));

                    Instruction *str_main_tracker_size = I->clone();
                    Value *main_tracker_size = ConstantInt::get(Type::getInt32Ty(M.getContext()), 0);
                    str_main_tracker_size->setOperand(0, main_tracker_size);
                    str_main_tracker_size->setOperand(1, M.getNamedValue("main_tracker"));
                    str_main_tracker_size->insertAfter(str_buff_size);

                  }
                }

                if (I->getOpcode() == Instruction::Call) {
                  
                  
                  // Insert calls to gather and scatter. 
                  CallSite cs(&(*I));
                  if (!cs.getInstruction()){
                    continue;
                  }
                  Value* called = cs.getCalledValue()->stripPointerCasts();
                  if (Function* f = dyn_cast<Function>(called)){
                    // if (f->getName().find("compute") != std::string::npos){
                    // if (f == compute_function){
                    if (f == compute_copy){

                      // Instruction *computeCall = I->clone();
                      
                      
                      // put back: std::string compute_name = f->getName();
                     //  std::string compute_name = "compute";
                     //  llvm::errs() << compute_name << "\n";

                      Function *main = &(*it); //Builder.GetInsertBlock()->getParent(); // Could prob just set it to it, but hey
                      // BasicBlock *PreheaderBB = Builder.GetInsertBlock();






                      BasicBlock *LoopCmpBB = BasicBlock::Create(M.getContext(), "while.cond", main); /// need to change this to come off det.comt
                      //BasicBlock *LoopCmpBB = BasicBlock::Create(M.getContext(), "while.cond", g0detc); /// need to change this to come off det.comt
                      BasicBlock *head = I->getParent(); 
                      BasicBlock *tail = head->splitBasicBlock(&*I, "while.endy-do"); 
                      head->getTerminator()->eraseFromParent();
                      Builder.SetInsertPoint(head);

                      Function *synctok = Intrinsic::getDeclaration(&M, Intrinsic::syncregion_start);
                      //Builder.CreateCall(synctok, None, "TESTESTTESTESTS");

                      // TODO: Insert initial calls to gather here
                      
                      BasicBlock *g0 = BasicBlock::Create(M.getContext(), "g0", main); 
                      BasicBlock *g0det = BasicBlock::Create(M.getContext(), "g0det", main); 
                      BasicBlock *g0detc = BasicBlock::Create(M.getContext(), "g0detc", main); 

                      Builder.SetInsertPoint(g0); 
                      LoadInst *load_main = Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker");
                      Instruction *g0det_inst = Builder.CreateDetach(g0det, g0detc, synctoken_g0);
                     
                      Builder.SetInsertPoint(g0det); 
                      std::vector<Value*> g0args;
                      g0args.push_back(dyn_cast<Value>(load_main));
                      g0args.push_back(dyn_cast<Value>(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0)));
                      Instruction *g0_call = Builder.CreateCall( M.getFunction("gather"), ArrayRef<Value*>(g0args));         
                      Builder.CreateReattach(g0detc, synctoken_g0); 

                      //Builder.SetInsertPoint(g0detc); 
                      //Builder.CreateBr(LoopCmpBB);
                      // Instead of breaking to while cond, insert the detach. 
                      
                      BasicBlock *g1 = g0detc; 
                      BasicBlock *g1det = BasicBlock::Create(M.getContext(), "g1det", main); 
                      BasicBlock *g1detc = BasicBlock::Create(M.getContext(), "g1detc", main); 

                      Builder.SetInsertPoint(g1); 
                      LoadInst *load_main2 = Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker");
                      LoadInst *ld_bf = Builder.CreateLoad(M.getNamedValue("buffer_size"), "ld_bf"); 
                      Value *ad_bf = Builder.CreateNSWAdd(load_main2, ld_bf);

                      Instruction *g1det_inst = Builder.CreateDetach(g1det, g1detc, synctoken_g1);
                     
                      Builder.SetInsertPoint(g1det); 
                      std::vector<Value*> g1args;
                      g1args.push_back(dyn_cast<Value>(ad_bf));
                      g1args.push_back(dyn_cast<Value>(ConstantInt::get(Type::getInt32Ty(M.getContext()), 1)));
                      Instruction *g1_call = Builder.CreateCall( M.getFunction("gather"), ArrayRef<Value*>(g1args));         
                      Builder.CreateReattach(g1detc, synctoken_g1); 

                      Builder.SetInsertPoint(g1detc); 
                      Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), Alloca_ct);
                      Builder.CreateBr(LoopCmpBB);


                      // load the main tracker, then call gather 0 and gather 1
                      //  
                      

                      // This is where you need to break to g0
                      Builder.SetInsertPoint(head);
                      //Builder.CreateBr(LoopCmpBB);
                      Builder.CreateBr(g0);

                      I->eraseFromParent();
                      // Not sure if it is important, but tail's pred is only loopcmp, and no longer includes ref to main 


                      Builder.SetInsertPoint(LoopCmpBB);
                      BasicBlock *LoopBB = BasicBlock::Create(M.getContext(), "while.body", main);
                      // BasicBlock *PostLoopBB = BasicBlock::Create(M.getContext(), "PostLoopBB", main);

                      Value *endCond = Builder.CreateICmpSLT(Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"),
                                      Builder.CreateLoad(M.getNamedValue("list_size"), "list_size"));

                      Builder.CreateCondBr(endCond, LoopBB, tail); // TODO: fix where to go 

                      //insert call to gather function before compute. 
                      Builder.SetInsertPoint(LoopBB);
                      
             



                 Value *ld_ct = Builder.CreateLoad(Alloca_ct);
                 Value *rem = Builder.CreateSRem(ld_ct, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
                 Value *cmp = Builder.CreateICmpEQ(rem, ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), "cmp");
                 
                  BasicBlock *if_then = BasicBlock::Create(M.getContext(), "if.then", main); 
                  BasicBlock *if_else = BasicBlock::Create(M.getContext(), "if.else", main);
                  
                  BasicBlock *if_then_cont = BasicBlock::Create(M.getContext(), "if.then.cont", main); 
                  BasicBlock *if_else_cont = BasicBlock::Create(M.getContext(), "if.else.cont", main);
                  
                  BasicBlock *it_sync = BasicBlock::Create(M.getContext(), "then.sync.cont", main); 
                  BasicBlock *ie_sync = BasicBlock::Create(M.getContext(), "else.sync.cont", main);
                  
                  BasicBlock *if_end = BasicBlock::Create(M.getContext(), "if.end", main); 

                  Builder.CreateCondBr(cmp, if_then, if_else);

                  Builder.SetInsertPoint(if_then);
                  Builder.CreateSync(if_then_cont, synctoken_g0);
                  Builder.SetInsertPoint(if_then_cont);
                  Builder.CreateSync(it_sync, synctoken_s0);
                  Builder.SetInsertPoint(it_sync);
                  Builder.CreateBr(if_end);


                  Builder.SetInsertPoint(if_else);
                  Builder.CreateSync(if_else_cont, synctoken_g1);
                  Builder.SetInsertPoint(if_else_cont);
                  Builder.CreateSync(ie_sync, synctoken_s1);
                  Builder.SetInsertPoint(ie_sync);
                  Builder.CreateBr(if_end);
                  
                 Builder.SetInsertPoint(if_end);



                  // TODO here:
                  // load main_tracker
                  // load compute_tracker
                  // remainder of compute_tracker % 2
                  // call compute (main tracker, remainder )
                  // comparre remainder to zero
                  // insert if else 


                 Value *ld_mn = Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"); 
                 ld_ct = Builder.CreateLoad(Alloca_ct);
                 rem = Builder.CreateSRem(ld_ct, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
                 cmp = Builder.CreateICmpEQ(rem, ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), "cmp");
                      
                 std::vector<Value*> args1;
                      args1.push_back(dyn_cast<Value>(ld_mn));
                      args1.push_back(dyn_cast<Value>(rem));
                Instruction *compute_call = Builder.CreateCall( M.getFunction("new_comput"), ArrayRef<Value*>(args1));
                  
               if_then = BasicBlock::Create(M.getContext(), "nu.if.then", main); 
               if_else = BasicBlock::Create(M.getContext(), "nu.if.else", main);
               
               if_then_cont = BasicBlock::Create(M.getContext(), "nu.if.then.cont", main); 
               if_else_cont = BasicBlock::Create(M.getContext(), "nu.if.else.cont", main);
                  
               it_sync = BasicBlock::Create(M.getContext(), "nu.then.sync.cont", main); 
               ie_sync = BasicBlock::Create(M.getContext(), "nu.else.sync.cont", main);
                 

              BasicBlock *sp_sc = BasicBlock::Create(M.getContext(), "spawn_scatter", main);
              BasicBlock *sp_ga = BasicBlock::Create(M.getContext(), "spawn_gather", main);
               
               if_end = BasicBlock::Create(M.getContext(), "nu.if.end", main); 

                Builder.CreateCondBr(cmp, if_then, if_else);

                  Builder.SetInsertPoint(if_then);

                ld_mn = Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"); 
                ld_bf = Builder.CreateLoad(M.getNamedValue("buffer_size"), "buffer_size"); 

                  Builder.CreateDetach(sp_sc, if_then_cont, synctoken_s0);
                  
                  Builder.SetInsertPoint(sp_sc);
                  Builder.CreateCall(M.getFunction("scatter"), ArrayRef<Value*>(args1));
                  Builder.CreateReattach(if_then_cont, synctoken_s0);

                  Builder.SetInsertPoint(if_then_cont);
                Value *m_bf = Builder.CreateNSWMul(ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), ld_bf); 
              Value *ad_mn_bf = Builder.CreateNSWAdd(ld_mn, m_bf);
              Builder.CreateDetach(sp_ga, it_sync, synctoken_g0);
                


             Builder.SetInsertPoint(sp_ga);
             
                 std::vector<Value*> g_args;
                      g_args.push_back(dyn_cast<Value>(ad_mn_bf));
                      g_args.push_back(dyn_cast<Value>(rem));
            
             Builder.CreateCall(M.getFunction("gather"), ArrayRef<Value*>(g_args));
              Builder.CreateReattach(it_sync, synctoken_g0);
                  Builder.SetInsertPoint(it_sync);
                  Builder.CreateBr(if_end);
                  


              sp_sc = BasicBlock::Create(M.getContext(), "else_spawn_scatter", main);
              sp_ga = BasicBlock::Create(M.getContext(), "else_spawn_gather", main);




                  Builder.SetInsertPoint(if_else);
                  ld_mn = Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"); 
                  ld_bf = Builder.CreateLoad(M.getNamedValue("buffer_size"), "buffer_size"); 
                  Builder.CreateDetach(sp_sc, if_else_cont, synctoken_s1);

                  
                  Builder.SetInsertPoint(sp_sc);
                  Builder.CreateCall(M.getFunction("scatter"), ArrayRef<Value*>(args1));
                  Builder.CreateReattach(if_else_cont, synctoken_s1);

                  
                  Builder.SetInsertPoint(if_else_cont);
                  m_bf = Builder.CreateNSWMul(ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), ld_bf); 
                  ad_mn_bf = Builder.CreateNSWAdd(ld_mn, m_bf);
                  Builder.CreateDetach(sp_ga, ie_sync, synctoken_g1);
                


             Builder.SetInsertPoint(sp_ga);
             
                 std::vector<Value*> g_args2;
                      g_args2.push_back(dyn_cast<Value>(ad_mn_bf));
                      g_args2.push_back(dyn_cast<Value>(rem));
            
             Builder.CreateCall(M.getFunction("gather"), ArrayRef<Value*>(g_args2));
              Builder.CreateReattach(ie_sync, synctoken_g1);
                  Builder.SetInsertPoint(ie_sync);
                  Builder.CreateBr(if_end);
                  
                  //Builder.CreateSync(ie_sync, synctoken_s1);
                  //Builder.SetInsertPoint(ie_sync);
                  //Builder.CreateBr(if_end);
                 
                  Builder.SetInsertPoint(if_end);

              //Builder.SetInsertPoint(if_end);






                
                   //  Instruction *gather_call = Builder.CreateCall( M.getFunction("gather"), ArrayRef<Value*>(args1));         
                     
                

   //                   Instruction *compute_call = Builder.CreateCall( M.getFunction("new_comput"), ArrayRef<Value*>(args1));

                      // Instruction *compute_call = Builder.CreateCall( M.getFunction(compute_name));
                      // Builder.CreateCall(compute_copy, ArrayRef<Value*>(args1));

                     //  Instruction *scatter_call = Builder.CreateCall( M.getFunction("scatter"), ArrayRef<Value*>(args1));

                      //Instruction *compute_call = Builder.CreateCall( M.getFunction(compute_name));
                      //Instruction *scatter_call = Builder.CreateCall( M.getFunction("scatter"));

                      Value *addition = Builder.CreateNSWAdd(Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"),
                                      Builder.CreateLoad(M.getNamedValue("buffer_size"), "buffer_size"));
                      Builder.CreateStore(addition, M.getNamedValue("main_tracker"));
                      
                      ld_ct = Builder.CreateLoad(Alloca_ct);
                      addition = Builder.CreateNSWAdd(ld_ct, ConstantInt::get(Type::getInt32Ty(M.getContext()), 1)); 
                      Builder.CreateStore(addition, if_end->getParent()->getValueSymbolTable()->lookup("compute_tracker"));
                        
                      Builder.CreateBr(LoopCmpBB);
                     

                      BasicBlock *sync_s0 = BasicBlock::Create(M.getContext(), "sync_s0", main);
                      BasicBlock *sync_s1 = BasicBlock::Create(M.getContext(), "sync_s1", main);
               
                      Builder.SetInsertPoint(sync_s0);
                      Builder.CreateSync(sync_s1, synctoken_s0);
                      
                      Builder.SetInsertPoint(sync_s1);
                      Builder.CreateSync(tail, synctoken_s1);
                      
                      Instruction *lp_cmp_term = LoopCmpBB->getTerminator();
                      lp_cmp_term->setOperand(1, sync_s0);
                        
                      // tail is the "while.end" with the store double 0 thing
                      //


                      
                      
  // This was working before, but is now obsolete.                     
                      
                      
  //                    BasicBlock *while_body = gather_call->getParent(); 
  //                    BasicBlock *sync_continue = while_body->splitBasicBlock(compute_call, "sync.continue"); 
  //                    while_body->getTerminator()->eraseFromParent();
  //                    Builder.SetInsertPoint(while_body);
  //                    Builder.CreateSync(sync_continue, synctoken_g0);
  //                    
  //                    BasicBlock *detached = sync_continue->splitBasicBlock(scatter_call, "detached"); 
  //                    sync_continue->getTerminator()->eraseFromParent();
  //                    Builder.SetInsertPoint(sync_continue);
  //                    Instruction *det_inst = Builder.CreateDetach(detached, detached, synctoken_g0);
  //                   
  //                    BasicBlock *det_cont = detached->splitBasicBlock(scatter_call->getNextNode(), "det.cont"); 
  //                    detached->getTerminator()->eraseFromParent();
  //                    Builder.SetInsertPoint(detached);
  //                    Builder.CreateReattach(det_cont, synctoken_g0);
  //                    det_inst->setOperand(1, det_cont);
  //                    

  //                    // split tail

  //                    BasicBlock *sec_sync = tail->splitBasicBlock(tail->getFirstNonPHI(), "final_sync_continue");
  //                    tail->getTerminator()->eraseFromParent();
  //                    Builder.SetInsertPoint(tail);
  //                    Builder.CreateSync(sec_sync, synctoken_g0);

                      



                      break;
                    } 
                  }
                }
              }
              // errs() << "after scatter " << *scatterF;

              

                  //errs() << M;

              // PUT THESE BACK
              verifyFunction(*it);
              verifyModule(M);

              // errs() << *it;

              // errs() << *computeF;
              // errs() << *gatherF;
              // errs() << *scatterF;
            }

/*
            for (inst_iterator I = inst_begin(*it), E = inst_end(*it); I != E; ++I){


                      BasicBlock *while_body = gather_call->getParent(); 
                      BasicBlock *sync_continue = while_body->splitBasicBlock(gather_call, "sync.continue"); 
                      while_body->getTerminator()->eraseFromParent();
                      Builder.SetInsertPoint(while_body);
                      Builder.CreateSync(sync_continue, synctoken);

          
            }
*/
          }

          //PUT THIS BACK
          verifyModule(M);

          //errs() << M;

          // debugging on local
          errs() << "writing result to file\n";

          std::error_code EC;
          raw_fd_ostream *Out = new raw_fd_ostream("/home/amaleewilson/pv_tests/test_out.bc", EC, sys::fs::F_None);
          WriteBitcodeToFile(&M, *Out);
          Out->flush();

          return true;
        }

        void getAnalysisUsage(AnalysisUsage &AU) const override{
          AU.addRequired<LoopInfoWrapperPass>();
        }

      };

    }



    char GatherScatter::ID = 0;
    static RegisterPass<GatherScatter> Z("gather", "Gather Pass attempt");


