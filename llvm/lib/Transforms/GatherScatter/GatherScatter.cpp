//===- GatherScatter.cpp - gather-scatter opt                     ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// 
// 
// 
// 
//
// 
// 
// 
//
//===----------------------------------------------------------------------===//

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

#include "llvm/Analysis/LoopInfo.h"

#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"

using namespace llvm;
// using namespace llvm::orc;

namespace {

  struct GatherScatter : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    GatherScatter() : ModulePass(ID) {}

    void add_while(Function* f, Module &M, BasicBlock* lp_body){
      IRBuilder<> Builder(M.getContext());


      Function *cF = f; //Builder.GetInsertBlock()->getParent(); // Could prob just set it to it, but hey

        Builder.SetInsertPoint(&(cF->getEntryBlock()));
        BasicBlock *PreBB = Builder.GetInsertBlock(); //Builder.GetInsertBlock();

        BasicBlock *LoopCBB = BasicBlock::Create(M.getContext(), "whilelpcond", cF);
        Instruction *split = PreBB->getFirstNonPHI(); 
        BasicBlock *head = split->getParent();
        BasicBlock *tail = head->splitBasicBlock(split); 
        head->getTerminator()->eraseFromParent();
        Builder.SetInsertPoint(head);
        AllocaInst *Alloca = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "tracker");
        Builder.CreateStore(Builder.CreateLoad(M.getNamedValue("main_tracker")), Alloca);

        Builder.CreateBr(LoopCBB);
        // I->eraseFromParent();
        // Not sure if it is important, but tail's pred is only loopcmp, and no longer includes ref to main 


        Builder.SetInsertPoint(LoopCBB);
        BasicBlock *LoopBB = BasicBlock::Create(M.getContext(), "whilelpend", cF);

        
        // BasicBlock *PostLoopBB = BasicBlock::Create(M.getContext(), "PostLoopBB", main);

        Value *trkr = cF->getValueSymbolTable()->lookup("tracker");

        Value *addition = Builder.CreateNSWAdd(Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"),
        Builder.CreateLoad(M.getNamedValue("buffer_size"), "buffer_size"));

        Value *endCond = Builder.CreateICmpSLT(Builder.CreateLoad(trkr, "trkr"),
          addition);

        Builder.CreateCondBr(endCond, tail, LoopBB); // TODO: fix where to go 

        // //insert call to gather function before compute. 
        Builder.SetInsertPoint(LoopBB);
        Builder.CreateBr(LoopCBB);


        for (inst_iterator I = inst_begin(cF), E = inst_end(cF); I != E; ++I){
          if (I->getOpcode() == Instruction::Ret){
            BasicBlock *end_block = I->getParent();
            BasicBlock *forlpcond = end_block->getSinglePredecessor();
            Instruction *to_mod = forlpcond->getTerminator();
            Value *to_end = to_mod->getOperand(1);
            to_mod->setOperand(1, LoopBB);
            Builder.SetInsertPoint(LoopBB);
            Instruction *whileEnd = LoopBB->getTerminator();
            whileEnd->setOperand(0, to_end);


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
      std::string idx_var = "";
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

      for (auto &globalVar : M.getGlobalList()){
        if (globalVar.getName() == gather_var){ 
          A = &globalVar;
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

      // Obviously this is not the best way to do this.
      gatherF = CloneFunction(compute_function, vmap);
      gatherF->setName("gather");

      Instruction *GEPg_A; //GEP for g_A[i] = ... 
      Instruction *ld_idx_A; // ... = A[idx[i]];
      Value *lp_i; // i in g_A[i]
      for (inst_iterator I = inst_begin(gatherF), E = inst_end(gatherF); I != E; ++I){

        if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP

          if (I->getOperand(0)->getName() == idx_var){
            lp_i = I->getOperand(2);
          }

          if (I->getOperand(0)->getName() == gather_var){
            GEPg_A = I->clone(); // Cloning GEP for A because we need to add a GEP for g_A
            // A[idx[i]] => g_A[i]
            // But need to preserve A[idx[i]] because we will store that val in g_A[i].
            GEPg_A->setOperand(0, g_A); // Fixing operand from A to g_A 
            GEPg_A->setOperand(2, lp_i); // Fixing operand from idx[i] to just i

            for (User *U : I->users()) {
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


            // Instruction *store_A = new StoreInst(ld_idx_A, GEPg_A, GEPg_A->getParent()->getTerminator());
            Instruction *store_A = new StoreInst(ld_idx_A, GEPg_A);
            store_A->insertBefore(GEPg_A->getParent()->getTerminator());


            add_while(gatherF, M, GEPg_A->getParent());
   

        }

      } 

    } // end instruction iterator on gatherF




          // Instruction *GEPg_A2; //GEP for g_A[i] = ... 
          Value *lp_i2; // i in g_A[i]
          // Instruction *idx_instr;
          computeF = compute_function;
          Instruction *str_in_s_A;
          Value *val_for_store;
          std::vector<Instruction*> to_delete; 

          for (inst_iterator I = inst_begin(computeF), E = inst_end(computeF); I != E; ++I){
            // Okay now replace A[idx[i]] += 1; to be g_A[i] += 1;
            // Now this needs to be s_A[i] = g_A[i] + 1; 

             if (I->getOpcode() == Instruction::GetElementPtr){ // if it is GEP, may be better way to do this. 

                if (I->getOperand(0)->getName() == "idx"){

                  lp_i2 = I->getOperand(2); // get third operand, which is i

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


          add_while(compute_function, M, str_in_s_A->getParent());
  

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


          verifyFunction(*computeF);
          verifyFunction(*gatherF);
          verifyFunction(*scatterF);
          verifyModule(M);

          for (Module::iterator it = M.begin(), end = M.end(); it != end; ++it){
            if (it->getName().find("main") != std::string::npos){
              for (inst_iterator I = inst_begin(*it), E = inst_end(*it); I != E; ++I){

                // Insert assignment for buffer size
                if (isa<StoreInst>(&(*I))) {
                  if (I->getOperand(1) == M.getNamedValue("list_size")) {
                    // errs() << "found store to list size" << '\n';

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
                    if (f == compute_function){

                      // Instruction *computeCall = I->clone();
                      std::string compute_name = f->getName();

                      Function *main = &(*it); //Builder.GetInsertBlock()->getParent(); // Could prob just set it to it, but hey
                      // BasicBlock *PreheaderBB = Builder.GetInsertBlock();

                      BasicBlock *LoopCmpBB = BasicBlock::Create(M.getContext(), "loopcmpBB", main);
                      BasicBlock *head = I->getParent(); 
                      BasicBlock *tail = head->splitBasicBlock(&*I); 
                      head->getTerminator()->eraseFromParent();
                      Builder.SetInsertPoint(head);
                      Builder.CreateBr(LoopCmpBB);
                      I->eraseFromParent();
                      // Not sure if it is important, but tail's pred is only loopcmp, and no longer includes ref to main 


                      Builder.SetInsertPoint(LoopCmpBB);
                      BasicBlock *LoopBB = BasicBlock::Create(M.getContext(), "LoopBB", main);
                      // BasicBlock *PostLoopBB = BasicBlock::Create(M.getContext(), "PostLoopBB", main);

                      Value *endCond = Builder.CreateICmpSLT(Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"),
                                      Builder.CreateLoad(M.getNamedValue("list_size"), "list_size"));

                      Builder.CreateCondBr(endCond, LoopBB, tail); // TODO: fix where to go 

                      //insert call to gather function before compute. 
                      Builder.SetInsertPoint(LoopBB);
                      Builder.CreateCall( M.getFunction("gather"));
                      Builder.CreateCall( M.getFunction(compute_name));
                      Builder.CreateCall( M.getFunction("scatter"));

                      Value *addition = Builder.CreateNSWAdd(Builder.CreateLoad(M.getNamedValue("main_tracker"), "main_tracker"),
                                      Builder.CreateLoad(M.getNamedValue("buffer_size"), "buffer_size"));
                      Builder.CreateStore(addition, M.getNamedValue("main_tracker"));
                      Builder.CreateBr(LoopCmpBB);

                      

                      break;
                    } 
                  }
                }
              }
              // errs() << "after scatter " << *scatterF;

              


              verifyFunction(*it);
              verifyModule(M);

              // errs() << *it;

              // errs() << *computeF;
              // errs() << *gatherF;
              // errs() << *scatterF;
            }
          }


          verifyModule(M);

          // errs() << M;

<<<<<<< HEAD
          // debugging on local
          // errs() << "writing result to file\n";
          // std::error_code EC;
          // raw_fd_ostream *Out = new raw_fd_ostream("/Users/amaleewilson/gs-pass/test/test_out.bc", EC, sys::fs::F_None);
          // WriteBitcodeToFile(&M, *Out);
          // Out->flush();
=======

          std::error_code EC;
          raw_fd_ostream *Out = new raw_fd_ostream("/Users/amaleewilson/gs-pass/test/test_out.bc", EC, sys::fs::F_None);
          WriteBitcodeToFile(&M, *Out);
          Out->flush();
>>>>>>> Initial version of gather-scatter pass, works on minimal example, produces squential gather-scatter

          return true;
        }

        void getAnalysisUsage(AnalysisUsage &AU) const override{
          AU.addRequired<LoopInfoWrapperPass>();
        }

      };

    }

    char GatherScatter::ID = 0;
    static RegisterPass<GatherScatter> Z("gather", "Gather Pass attempt");

