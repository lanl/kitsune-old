//===- GatherScatter.cpp - gather-scatter opt                     ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// 
// This file looks for an annotated loop and generates a gather and scatter
// method based on the function that the annotated loop is found in. 
// Calls to gather and scatter are generated and rely on Tapir constructs
// to synchronize the gathering, scattering, and compute. 
// While loops are added in the functions (including main) in order to 
// facilitate more complex data access in the future. The original algorithm 
// used such while loops to account for conditionals.
// Current prototype is pretty limited and a bit kludgey, but lays the 
// foundation for a transformation that might do somehting like an auto-
// parallelized gather-scatter code targeting some parallel IR.
//
// It is likely that there are better ways to achieve some of what is done
// in this transformation, and there is certainly need to remove current 
// hard-coded things (e.g. buffer count, double type), which were 
// short-cutted for the sake of completing the prototype in a timely fashion. 
//
// In the event that another person is reading this code and has questions, 
// my permanent email is at gmail: amaleewilson. 
//
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

  GlobalVariable* buffer_size;
  GlobalVariable* L_size;
  GlobalVariable* main_tracker;


  std::string idx_var;
  unsigned lsize = 1;

  struct GatherScatter : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    GatherScatter() : ModulePass(ID) {}

    /*
     * The while loops found in the gather, scatter and compute are currently redundant.
     * However, they become useful later for sparse access, e.g. if there is a conditional 
     * that affects which things are gathered, we want to only gather if that conditional
     * is satisfied. In other words, we may iterate over 50 elements and gather only 10. 
     *
     * This sort of logic is not currently supported, but the current implemenetation  
     * is designed to be extensible should this be pursued further. 
     * 
     * f: Function in which we want to add a while loop.
     * M: Module that contains the function.
     * lp_body: The loops that we want to surround with a while loop.
     *
     * Note that this has been tested only for a lp_body that is a for loop and the only
     * thing inside the function f. 
     */
    void add_while(Function* f, Module &M, BasicBlock* lp_body){
      IRBuilder<> Builder(M.getContext());

      // Get the function arguments, which describe where you are in the original data 
      // structure and which buffer to collect into. 
      Function::arg_iterator args = f->arg_begin();
      // track_arg is where you are in og data structure
      Value* track_arg = args++; 
      // loc_arg is the buffer into which you gather data
      Value* loc_arg = args++;

      // Splitting entry block to surround the whole function with the while loop
      BasicBlock *entryBB = &(f->getEntryBlock()); 
      BasicBlock *while_cond = BasicBlock::Create(M.getContext(), "whilelpcond", f);
      BasicBlock *while_body = entryBB->splitBasicBlock(entryBB->getFirstNonPHI(), "while.body"); 

      Instruction *entry_term = entryBB->getTerminator(); 
      Builder.SetInsertPoint(entry_term);

      // important while loop vars
      AllocaInst *track = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "track");
      Builder.CreateStore(track_arg, track);
      AllocaInst *loc = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "loc");
      Builder.CreateStore(loc_arg, loc);
      AllocaInst *Alloca = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "tracker");
      Builder.CreateStore(track_arg, Alloca);

      // Fixing structure
      Builder.CreateBr(while_cond);
      entry_term->eraseFromParent();

      Builder.SetInsertPoint(while_cond);
      BasicBlock *while_end = BasicBlock::Create(M.getContext(), "whilelpend", f);

      // Values for while loop iterations
      Value *trkr = f->getValueSymbolTable()->lookup("tracker");
      Value *addition = Builder.CreateNSWAdd(track_arg,
          Builder.CreateLoad(buffer_size, "buffer_size"));
      Value *endCond = Builder.CreateICmpSLT(Builder.CreateLoad(trkr, "trkr"), addition);

      // Structure
      BasicBlock *land = BasicBlock::Create(M.getContext(), "land.rhs", f, while_body);
      BasicBlock *landend = BasicBlock::Create(M.getContext(), "land.end", f, while_body);
      Builder.CreateCondBr(endCond, land, landend);

      // Looping conditions
      Builder.SetInsertPoint(land);
      LoadInst *load_trkr = Builder.CreateLoad(trkr, "trkr");
      LoadInst *load_lst = Builder.CreateLoad(L_size, "lst_size");
      Value *sub = Builder.CreateNSWSub(load_lst, ConstantInt::get(Type::getInt32Ty(M.getContext()), 1), "sub");
      Value *cmp2 = Builder.CreateICmpSLT(load_trkr, sub); 
      Builder.CreateBr(landend);

      // Structure
      Builder.SetInsertPoint(landend);
      PHINode *PN = Builder.CreatePHI(Type::getInt1Ty(M.getContext()), 2, "pn");
      PN->addIncoming(ConstantInt::get(Type::getInt1Ty(M.getContext()), 0), while_cond);
      PN->addIncoming(cmp2, land);
      Builder.CreateCondBr(PN, while_body, while_end);
      Builder.SetInsertPoint(while_end);
      Builder.CreateBr(while_cond);

      // Above code is mostly just setting up the structure and relevant instructions for the while loop logic


      // This loop finds the return instruction of the funcion
      Instruction *fn_ret;
      for (inst_iterator I = inst_begin(f), E = inst_end(f); I != E; ++I){
        if (I->getOpcode() == Instruction::Ret){
          fn_ret = &*I;
          break;
        }
      }

      // This is mostly structural and swaps the previous break with the return.       
      Instruction *ret_void = fn_ret->clone();
      Instruction *whileEnd = while_end->getTerminator();
      Instruction *br_while_cond = whileEnd->clone();

      br_while_cond->insertBefore(fn_ret);
      fn_ret->eraseFromParent();

      ret_void->insertBefore(whileEnd);
      whileEnd->eraseFromParent();

      // This loop is changing the loop condition so that it iterates buffer_size times
      // instead of list_size times. 
      // There must be a better way to change the loop condition
      for (inst_iterator I = inst_begin(f), E = inst_end(f); I != E; ++I){
        if (I->getOpcode() == Instruction::ICmp){
          Value *N = I->getOperand(1);
          // This only works when a constant known at compile time is used at the term for the for loop. 
          // TODO: fix this to be able to use a variable 
          ConstantInt *lstsz = dyn_cast<llvm::ConstantInt>(N);
          if (lstsz){
            if (lstsz->equalsInt(lsize)){
              Builder.SetInsertPoint(&*I);
              Instruction *ld_bsize = Builder.CreateLoad(buffer_size);
              I->setOperand(1, ld_bsize);
            }
          }
        }
      }

      // if gather and A, replace A[idx[i]] with A[idx[tracker]]
      // if compute, leave g_A[i] and s_A[i]
      // if scatter and A, replace A[idx[i]] with A[idx[tracker]]
      for (inst_iterator I = inst_begin(f), E = inst_end(f); I != E; ++I){
        if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP
          if (I->getOperand(0)->getName() == idx_var){
            Builder.SetInsertPoint(&*I);
            Value *t_load = Builder.CreateLoad(trkr, "trkr");
            I->setOperand(2, t_load);
          }
        } 
      }

      // Just incrementing the while loop
      Instruction *lp_bod_term = lp_body->getTerminator();
      Builder.SetInsertPoint(lp_bod_term);
      Builder.CreateStore(Builder.CreateNSWAdd(Builder.CreateLoad(trkr), 
            ConstantInt::get(Type::getInt32Ty(M.getContext()), 1)), trkr);

    }

    bool runOnModule(Module &M) override {
      errs() << "running on module\n";

      std::string gather_var = "";
      std::string scatter_var = "";
      idx_var = "";
      unsigned bsize = 1;

      MDNode *meta_data; 
      Function *compute_function;

      // This loop is looking for a loop in the code with the pipevec metadata.  
      // There may be a better way to get this loop. 
      // Also, this basically enforces that there be exactly one pipevec loop in the module,
      // which is not necessarily something we want to do. 
      for (auto F = M.getFunctionList().begin(), 
          eF = M.getFunctionList().end(); 
          F != eF; ++F) {

        if ((*F).size() == 0) continue;

        for (Function::iterator b = F->begin(), be = F->end(); b != be; ++b) {
          LoopInfo &li = getAnalysis<LoopInfoWrapperPass>(*F).getLoopInfo();
          BasicBlock *BB = &*b;
          Loop *lp = li.getLoopFor(BB);

          if (lp){
            MDNode *LoopID = lp->getLoopID();
            if (LoopID){
              meta_data = GetUnrollMetadata(LoopID, "llvm.loop.pv.enable");
              if (meta_data){
                MDNode *MD = dyn_cast<MDNode>(LoopID->getOperand(1));
                compute_function = BB->getParent();

                MDString *gather_var_name = dyn_cast<MDString>(MD->getOperand(1));
                if (gather_var_name){
                  gather_var = gather_var_name->getString();
                }
                MDString *scatter_var_name = dyn_cast<MDString>(MD->getOperand(2));
                if (scatter_var_name){
                  scatter_var = scatter_var_name->getString();
                }
                MDString *idx_var_name = dyn_cast<MDString>(MD->getOperand(3));
                if (idx_var_name){
                  idx_var = idx_var_name->getString();
                }

                // This assumes that buffer size always comes before list size, which
                // may not be an good assumption. 
                bsize = mdconst::extract<ConstantInt>(MD->getOperand(4))->getZExtValue();
                lsize = mdconst::extract<ConstantInt>(MD->getOperand(5))->getZExtValue();
                break;
                //    llvm::errs() << "bsize " << bsize << "\n";
                //    llvm::errs() << "lsize " << lsize << "\n";
              }
            } //end if(LoopID)
          } // end if(lp)
        } // end bb iter
      } // end func iter


      /*
       * BEGIN buffer creation
       * This section creates 2 gather and 2 scatter buffer on the stack. 
       * TODO: Should conform to whatever the type is of the thing being gathered/scattered,
       * but it currently is supporting only doubles. 
       * Setting the alignment to 8 or 16 only because the original version did so. May not be optimal or portable. 
       */
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

      Constant *gep_const = ConstantExpr::getGetElementPtr(ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2), 
          gA_buffs, index_vector);

      buff_q->setInitializer(gep_const);

      GlobalVariable* sA_buffs = new GlobalVariable(M, ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2),
          false, GlobalValue::CommonLinkage, 0, "sAbuffs");

      sA_buffs->setAlignment(16); 
      ConstantAggregateZero* sconst_array_2 = ConstantAggregateZero::get(ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2));

      sA_buffs->setInitializer(sconst_array_2);

      GlobalVariable* sbuff_q = new GlobalVariable(M, PointerType::get(Type::getDoublePtrTy(M.getContext()), 0),
          false, GlobalValue::ExternalLinkage, 0, "sbuff_q"); // correct linkage here?  

      sbuff_q->setAlignment(8); 

      std::vector<Value*> sindex_vector;
      sindex_vector.push_back((Value*)ConstantInt::get(Type::getInt32Ty(M.getContext()), 0));
      sindex_vector.push_back((Value*)ConstantInt::get(Type::getInt32Ty(M.getContext()), 0));

      Constant *sgep_const = ConstantExpr::getGetElementPtr(
          ArrayType::get(Type::getDoublePtrTy(M.getContext()), 2), 
          sA_buffs,
          sindex_vector);

      sbuff_q->setInitializer(sgep_const);
      /* END buffer creation */

      // Global variables to store list size, buffer size, and the main_tracker, which tracks location in og data. 
      L_size = new GlobalVariable(M, Type::getInt32Ty(M.getContext()), false, GlobalVariable::ExternalLinkage,
          ConstantInt::get(Type::getInt32Ty(M.getContext()), lsize), "L_size");
      buffer_size = new GlobalVariable(M, Type::getInt32Ty(M.getContext()), false, GlobalVariable::ExternalLinkage,
          ConstantInt::get(Type::getInt32Ty(M.getContext()), bsize), "buffer_size");
      main_tracker = new GlobalVariable(M, Type::getInt32Ty(M.getContext()), false, GlobalVariable::ExternalLinkage,
          ConstantInt::get(Type::getInt32Ty(M.getContext()), lsize), "main_tracker");

      /* 
       * Consider using this loop to get types of the gather variables. 
       */
      //for (auto &globalVar : M.getGlobalList()){
      //  if (globalVar.getName() == gather_var){ 
      //    continue;
      //  }
      //}


      /*
       * In this section, the compute method is modified to take two arguments.
       * Previous calls to compute are replaced with "new_comput" which takes two args.
       */
      IRBuilder<> Builder(M.getContext());

      std::vector<Type*> new_args(2, Type::getInt32Ty(M.getContext()));
      FunctionType *FT = FunctionType::get(Type::getVoidTy(M.getContext()), new_args, false);
      Function *compute_copy = Function::Create(FT, Function::ExternalLinkage, "new_comput", &M);

      llvm::ValueToValueMapTy vmap3;
      llvm::SmallVector<llvm::ReturnInst*, 2> Returns;
      CloneFunctionInto(compute_copy, compute_function, vmap3, 0, Returns);
      Function::arg_iterator args = compute_copy->arg_begin();
      Value* x = args++;
      x->setName("track");
      Value* y = args++;
      y->setName("loc");
      Instruction *test_c_call;

      Instruction *compute_call_to_delete;
      for (User *U : compute_function->users()) {
        if (Instruction *Inst = dyn_cast<Instruction>(U)) {
          for (Use &U2 : Inst->operands()) {
            Value *v = U2.get();
            if (v == compute_function){

              std::vector<Value*> args1;
              // TODO: use correct arguments here. 
              args1.push_back(dyn_cast<Value>(ConstantInt::get(Type::getInt32Ty(M.getContext()), lsize)));
              args1.push_back(dyn_cast<Value>(new LoadInst(main_tracker, "load main tracker", Inst)));
              Builder.SetInsertPoint(Inst);
              test_c_call = Builder.CreateCall(compute_copy, ArrayRef<Value*>(args1));
              compute_call_to_delete = Inst;
            }
          }        
        }
      }
      compute_call_to_delete->eraseFromParent();

      /*
       * This section initializes the buffers. 
       * It is placed here because the buffers should be initialized right before
       * the call to compute. With that said, TODO: buffers should be initalized
       * only once, and this code may get weird if compute is called multiple times.  
       *
       * Also, another unfortunate thing about this is that it relies on the file
       * having some init_buffer function with the given signature. 
       * Maybe it's possible to generate this init_buffer function?
       */
      Builder.SetInsertPoint(test_c_call);
      FunctionType *ib_ft ;
      Constant *init_buff;
      Function *ib_func;
      Instruction *call ;
      Instruction *ld_bq;
      ConstantInt *ld_i;
      Value *array_idx ;

      for (int i = 0; i < 2; ++i){

        std::vector<Type*> init_buffer_arg_types(1, Type::getInt32Ty(M.getContext()));
        ib_ft = FunctionType::get(Type::getDoublePtrTy(M.getContext()), init_buffer_arg_types, false);

        std::vector<Value*> init_buffer_args;

        init_buffer_args.push_back(dyn_cast<Value>(new LoadInst(buffer_size, "load buff 1", test_c_call)));

        init_buff = M.getOrInsertFunction("init_double_buffer", ib_ft);
        ib_func = dyn_cast<Function>(init_buff);

        call = Builder.CreateCall(ib_func, ArrayRef<Value*>(init_buffer_args));
        ld_bq = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
        ld_i = ConstantInt::get(Type::getInt32Ty(M.getContext()), i);
        array_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), ld_bq, ld_i, "arrayidx");
        Builder.CreateStore(call, array_idx, false);

        call = Builder.CreateCall(ib_func, ArrayRef<Value*>(init_buffer_args));
        ld_bq = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "ld_sbq" );
        array_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), ld_bq, ld_i, "arrayidx");
        Builder.CreateStore(call, array_idx, false);
      }
      /* END buffer initialization */

      Function *gatherF;
      llvm::ValueToValueMapTy vmap;
      Function *computeF;
      Function *scatterF;
      llvm::ValueToValueMapTy s_vmap; 


      gatherF = CloneFunction(compute_copy, vmap);
      gatherF->setName("gather");

      compute_function->eraseFromParent();

      Instruction *GEPg_A; //GEP for g_A[i] = ... 
      Instruction *ld_idx_A; // ... = A[idx[i]];
      Value *lp_i; // i in g_A[i]

      Instruction *old_inst;

      for (inst_iterator I = inst_begin(gatherF), E = inst_end(gatherF); I != E; ++I){
        if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP
          if (I->getOperand(0)->getName() == idx_var){ // if it is operating on idx var
            //llvm::errs() << "inst using idx var " << *I << "\n";
            lp_i = I->getOperand(2); // get the operand, i.e. loop index var
            //llvm::errs() << "lp_i " << *lp_i << "\n";
          }

          if (I->getOperand(0)->getName() == gather_var){ // if it is operating on gather var
            old_inst = &*I;
            //llvm::errs() << "old_inst " << *old_inst << "\n";
            GEPg_A = I->clone(); // Cloning GEP for A because we need to add a GEP for g_A
          }
        } 
      } // end instruction iterator on gatherF

      Instruction *gld_buffer_q;
      Value *grem ;
      Value *gbuff_idx;
      Instruction *ld_gbf_idx;

      Function::arg_iterator gargs = gatherF->arg_begin();
      gargs++;
      Value* loc_arg = gargs++;

      //llvm::errs() << *gatherF;

      // consider putting this back. 
      //old_inst->setOperand(2, lp_i); 

      //llvm::errs() << "GEPg_A pre setOp " << *GEPg_A << "\n";
      GEPg_A->setOperand(2, lp_i); // Fixing operand from idx[i] to just i
      //llvm::errs() << "GEPg_A post setOp " << *GEPg_A << "\n";
      // TODO: make this better  
      for (User *U : old_inst->users()) {
        if (Instruction *Inst = dyn_cast<Instruction>(U)) {
          if (isa<LoadInst>(Inst)) {
            // grab the load for the operand for g_A's store. 
            ld_idx_A = Inst;
            //llvm::errs() << "ld_idx_A " << *ld_idx_A << "\n";

            // delete everything that uses the load
            for (User *LU : ld_idx_A->users()) {
              if (Instruction *LInst = dyn_cast<Instruction>(LU)) {
                //llvm::errs() << "LInst to erase " << *LInst << "\n";
                LInst->eraseFromParent();
              }
            }
          }
          // delete stores to A
          else if (isa<StoreInst>(Inst)){
            //llvm::errs() << "Store Inst to erase " << *Inst << "\n";
            Inst->eraseFromParent();
          }
        }
      }

      //llvm::errs() << *gatherF;

      // TODO: use the builder to insert instructions instead!!!! 
      GEPg_A->insertAfter(ld_idx_A); // insert after load

      //llvm::errs() << "before buffer logic \n" << *gatherF;

      Builder.SetInsertPoint(ld_idx_A->getNextNode());
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
      grem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, grem, "buff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "load_buff_idx"); 

      Value *gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, lp_i, "gep_at_bf");
      Builder.CreateStore(ld_idx_A, gep_at_bf, "store_A_new_buff");

      //llvm::errs() << "before add while \n" << *gatherF;
      add_while(gatherF, M, GEPg_A->getParent());
      //llvm::errs() << "after add while \n" << *gatherF;

      scatterF = CloneFunction(compute_copy, s_vmap);
      scatterF->setName("scatter");


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
      sargs++;
      loc_arg = sargs++;


      Builder.SetInsertPoint(gep_idx_var);
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "sld_bq" );
      grem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "srem"); 
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, grem, "sbuff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "sload_buff_idx"); 

      gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, gep_idx_var->getOperand(2), "sgep_at_bf");
      Value *sload = Builder.CreateLoad(Type::getDoubleTy(M.getContext()), gep_at_bf, "sld" );

      Builder.SetInsertPoint(gep_idx_var->getParent()->getTerminator());
      Builder.CreateStore(sload, gep_gather_var, "sstore_A_new_buff");

      add_while(scatterF, M, gep_idx_var->getParent());


      // warning Value *lp_i2; // i in g_A[i]
      computeF = compute_copy;

      // warning Instruction *str_in_s_A;
      // warning Value *val_for_store;
      std::vector<Instruction*> to_delete; 

      // warning Instruction *ld_buffer_q;
      // warning Value *q_location;
      Value *rem ;
      // warning Value *buff_idx;

      Function::arg_iterator cargs = computeF->arg_begin();
      cargs++;
      loc_arg = cargs++;

      Instruction *idx_gep_inst;
      Instruction *gat_gep_inst;
      Instruction *store_res;

      Instruction *sld_buffer_q;
      Value *sbuff_idx;
      Value *gep_at_sbf;
      Instruction *ld_sbf_idx;
      // warning Value *srem;
      Instruction *ld_frm_A;
      Instruction *icmp;

      for (inst_iterator I = inst_begin(computeF), E = inst_end(computeF); I != E; ++I){
        // Okay now replace A[idx[i]] += 1; to be g_A[i] += 1;
        // Now this needs to be s_A[i] = g_A[i] + 1; 

        if (I->getOpcode() == Instruction::GetElementPtr){
          if (I->getOperand(0)->getName() == idx_var){ 
            idx_gep_inst = &*I;
          }
          if (I->getOperand(0)->getName() == gather_var){ 
            gat_gep_inst = &*I; 
          }
        } 

        if (isa<StoreInst>(dyn_cast<Instruction>(&*I))){
          if(I->getOperand(1) == gat_gep_inst){
            store_res = &*I;
          }
        }
        if (isa<LoadInst>(dyn_cast<Instruction>(&*I))){
          if(I->getOperand(0) == gat_gep_inst){
            ld_frm_A = &*I;
          }
        }
        if (isa<ICmpInst>(dyn_cast<Instruction>(&*I))){
          icmp = &*I;
        }
      }

      Builder.SetInsertPoint(&*(inst_begin(computeF)));
      Value *ld_bsize_icmp = Builder.CreateLoad(Type::getInt32Ty(M.getContext()), buffer_size, "icmp_bsize");

      icmp->setOperand(1, ld_bsize_icmp);      

      Builder.SetInsertPoint(idx_gep_inst); 
      rem = Builder.CreateSRem(loc_arg, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 

      // gather stuff
      gld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), buff_q, "ld_bq" );
      gbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), gld_buffer_q, rem, "buff_idx"); 
      ld_gbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), gbuff_idx, "load_buff_idx"); 
      gep_at_bf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()) , ld_gbf_idx, idx_gep_inst->getOperand(2), "gep_at_bf");
      ld_frm_A->setOperand(0, gep_at_bf);

      //scatter stuff
      sld_buffer_q = Builder.CreateLoad(PointerType::get(Type::getDoublePtrTy(M.getContext()), 0), sbuff_q, "sld_bq" );
      sbuff_idx = Builder.CreateGEP(Type::getDoublePtrTy(M.getContext()), sld_buffer_q, rem, "sbuff_idx"); 
      ld_sbf_idx = Builder.CreateLoad(Type::getDoublePtrTy(M.getContext()), sbuff_idx, "sload_buff_idx"); 

      gep_at_sbf = Builder.CreateGEP(Type::getDoubleTy(M.getContext()), ld_sbf_idx, idx_gep_inst->getOperand(2), "sgep_at_bf");
      sload = Builder.CreateLoad(Type::getDoubleTy(M.getContext()), gep_at_sbf, "sld" );
      store_res->setOperand(1, gep_at_sbf);


      add_while(computeF, M, store_res->getParent());

      verifyFunction(*computeF);
      verifyFunction(*gatherF);
      verifyFunction(*scatterF);
      verifyModule(M);

      Instruction *synctoken_g0;
      Instruction *synctoken_g1;
      Instruction *synctoken_s0;
      Instruction *synctoken_s1;

      AllocaInst *Alloca_ct; 
      Function *main;

      for (Module::iterator it = M.begin(), end = M.end(); it != end; ++it){
        if (it->getName().find("main") != std::string::npos){
          main = &(*it);
        }
      }

      // Set initial values for buffer size, list size, and main tracker 
      BasicBlock *main_entry = &(main->getEntryBlock());
      Builder.SetInsertPoint(main_entry->getFirstNonPHI());
      Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), bsize), buffer_size);
      Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), lsize), L_size);
      Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), main_tracker);


      // Make all the sync tokens.
      Function *synctok = Intrinsic::getDeclaration(&M, Intrinsic::syncregion_start);
      synctoken_g0 = Builder.CreateCall(synctok, None, "synctoken_g0");
      synctoken_g1 = Builder.CreateCall(synctok, None, "synctoken_g1");
      synctoken_s0 = Builder.CreateCall(synctok, None, "synctoken_s0");
      synctoken_s1 = Builder.CreateCall(synctok, None, "synctoken_s1");

      // Need to track buffer for compute.
      Alloca_ct = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "compute_tracker");
      Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), Alloca_ct);                      

      // Here we find the call to compute in main.
      Instruction *call_to_cmp;
      for (inst_iterator I = inst_begin(main), E = inst_end(main); I != E; ++I){
        if (I->getOpcode() == Instruction::Call) {
          CallSite cs(&(*I));
          if (!cs.getInstruction()){
            continue;
          }
          Value* called = cs.getCalledValue()->stripPointerCasts();
          if (Function* f = dyn_cast<Function>(called)){
            if (f == compute_copy){
              call_to_cmp = &*I;
              break;
            } 
          }
        }
      }


      /*
       * This section inserts a while loop in main and handles a crazy amount of restructuring
       * so that the detaches, reattaches, and syncs can be added. 
       * Many new basic blocks are inserted, and this section is long and complicated.
       * It should be possible to generalize at least some of this section, which ought to 
       * make it look a lot less ugly.
       */
      BasicBlock *LoopCmpBB = BasicBlock::Create(M.getContext(), "while.cond", main); /// need to change this to come off det.comt
      BasicBlock *head = call_to_cmp->getParent(); 
      BasicBlock *tail = head->splitBasicBlock(call_to_cmp, "while.end"); 
      head->getTerminator()->eraseFromParent();
      Builder.SetInsertPoint(head);

      BasicBlock *g0 = BasicBlock::Create(M.getContext(), "g0", main); 
      BasicBlock *g0det = BasicBlock::Create(M.getContext(), "g0det", main); 
      BasicBlock *g0detc = BasicBlock::Create(M.getContext(), "g0detc", main); 

      Builder.SetInsertPoint(g0); 
      LoadInst *load_main = Builder.CreateLoad(main_tracker, "main_tracker");
      /* warning Instruction *g0det_inst =*/ Builder.CreateDetach(g0det, g0detc, synctoken_g0);

      Builder.SetInsertPoint(g0det); 
      std::vector<Value*> g0args;
      g0args.push_back(dyn_cast<Value>(load_main));
      g0args.push_back(dyn_cast<Value>(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0)));
      /*warning Instruction *g0_call =*/ Builder.CreateCall( M.getFunction("gather"), ArrayRef<Value*>(g0args));         
      Builder.CreateReattach(g0detc, synctoken_g0); 

      // Instead of breaking to while cond, insert the detach. 

      BasicBlock *g1 = g0detc; 
      BasicBlock *g1det = BasicBlock::Create(M.getContext(), "g1det", main); 
      BasicBlock *g1detc = BasicBlock::Create(M.getContext(), "g1detc", main); 

      Builder.SetInsertPoint(g1); 
      LoadInst *load_main2 = Builder.CreateLoad(main_tracker, "main_tracker");
      LoadInst *ld_bf = Builder.CreateLoad(buffer_size, "ld_bf"); 
      Value *ad_bf = Builder.CreateNSWAdd(load_main2, ld_bf);

      Builder.CreateDetach(g1det, g1detc, synctoken_g1);

      Builder.SetInsertPoint(g1det); 
      std::vector<Value*> g1args;
      g1args.push_back(dyn_cast<Value>(ad_bf));
      g1args.push_back(dyn_cast<Value>(ConstantInt::get(Type::getInt32Ty(M.getContext()), 1)));
      Builder.CreateCall( M.getFunction("gather"), ArrayRef<Value*>(g1args));         
      Builder.CreateReattach(g1detc, synctoken_g1); 

      Builder.SetInsertPoint(g1detc); 
      Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), Alloca_ct);
      Builder.CreateBr(LoopCmpBB);

      // load the main tracker, then call gather 0 and gather 1

      // This is where you need to break to g0
      Builder.SetInsertPoint(head);
      //Builder.CreateBr(LoopCmpBB);
      Builder.CreateBr(g0);

      call_to_cmp->eraseFromParent();
      // Not sure if it is important, but tail's pred is only loopcmp, and no longer includes ref to main 


      Builder.SetInsertPoint(LoopCmpBB);
      BasicBlock *LoopBB = BasicBlock::Create(M.getContext(), "while.body", main);

      Value *endCond = Builder.CreateICmpSLT(Builder.CreateLoad(main_tracker, "main_tracker"),
          Builder.CreateLoad(L_size, "list_size"));

      Builder.CreateCondBr(endCond, LoopBB, tail); 

      //insert call to gather function before compute. 
      Builder.SetInsertPoint(LoopBB);

      Value *ld_ct = Builder.CreateLoad(Alloca_ct);
      Value *rem2 = Builder.CreateSRem(ld_ct, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
      Value *cmp = Builder.CreateICmpEQ(rem2, ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), "cmp");

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

      Value *ld_mn = Builder.CreateLoad(main_tracker, "main_tracker"); 
      ld_ct = Builder.CreateLoad(Alloca_ct);
      rem2 = Builder.CreateSRem(ld_ct, ConstantInt::get(Type::getInt32Ty(M.getContext()), 2), "rem"); 
      cmp = Builder.CreateICmpEQ(rem2, ConstantInt::get(Type::getInt32Ty(M.getContext()), 0), "cmp");

      std::vector<Value*> args1;
      args1.push_back(dyn_cast<Value>(ld_mn));
      args1.push_back(dyn_cast<Value>(rem2));
      /* warning unused Instruction *compute_call =*/  Builder.CreateCall( M.getFunction("new_comput"), ArrayRef<Value*>(args1));

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

      ld_mn = Builder.CreateLoad(main_tracker, "main_tracker"); 
      ld_bf = Builder.CreateLoad(buffer_size, "buffer_size"); 

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
      g_args.push_back(dyn_cast<Value>(rem2));

      Builder.CreateCall(M.getFunction("gather"), ArrayRef<Value*>(g_args));
      Builder.CreateReattach(it_sync, synctoken_g0);
      Builder.SetInsertPoint(it_sync);
      Builder.CreateBr(if_end);

      sp_sc = BasicBlock::Create(M.getContext(), "else_spawn_scatter", main);
      sp_ga = BasicBlock::Create(M.getContext(), "else_spawn_gather", main);

      Builder.SetInsertPoint(if_else);
      ld_mn = Builder.CreateLoad(main_tracker, "main_tracker"); 
      ld_bf = Builder.CreateLoad(buffer_size, "buffer_size"); 
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
      g_args2.push_back(dyn_cast<Value>(rem2));

      Builder.CreateCall(M.getFunction("gather"), ArrayRef<Value*>(g_args2));
      Builder.CreateReattach(ie_sync, synctoken_g1);
      Builder.SetInsertPoint(ie_sync);
      Builder.CreateBr(if_end);

      Builder.SetInsertPoint(if_end);

      Value *addition = Builder.CreateNSWAdd(Builder.CreateLoad(main_tracker, "main_tracker"),
          Builder.CreateLoad(buffer_size, "buffer_size"));
      Builder.CreateStore(addition, main_tracker);

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
      /* END adding while loop in main and major restructuring for parallelism*/

      verifyFunction(*main);


      // errs() << *computeF;
      // errs() << *gatherF;
      // errs() << *scatterF;

      verifyModule(M);

      //errs() << M;

      // debugging, this is obv not a general solution. 
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

