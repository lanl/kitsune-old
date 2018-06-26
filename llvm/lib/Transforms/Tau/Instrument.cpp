//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h" // For errs(), I think
#include "llvm/IR/IRBuilder.h" // IRBuilder
#include "llvm/IR/Constants.h" // To create string literals in the AST

// Need these to use Sampson's registration technique
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"


using namespace llvm;


namespace {

  /*!
   *  Find/create a function taking no arguments with a void return type
   *  suitable for making a call to in IR.
   *
   * \param funcname The name of the function
   * \param ctx The LLVMContext
   * \param mdl The Module in which the function will be used
   */
  static Constant *getVoidFunc(StringRef funcname, LLVMContext &context, Module *module) {

    // Function has Void return
    Type *retTy = Type::getVoidTy(context);
    Type *argTy = Type::getInt8PtrTy(context);

    // first cut: Function takes no args
    std::vector<Type *> paramTys{argTy};

    // Third param here is `isVarArg`.  It's not documented, but might have to
    // do with variadic functions?
    FunctionType *funcTy = FunctionType::get(retTy, paramTys, false);
    return module->getOrInsertFunction(funcname, funcTy);
  }


  /*!
   * The instrumentation pass.
   *
   */
  struct Instrument : public FunctionPass {

    static char ID; // Pass identification, replacement for typeid
    Instrument() : FunctionPass(ID) {}

    bool runOnFunction(Function &func) override {

      errs() << "I've been called on some function!\n";

      bool mutated = false;

      // Declare and get handles to the runtime profiling functions
      auto &context = func.getContext();
      auto *module = func.getParent();
      Constant
        *onCallFunc = getVoidFunc("tau_prof_func_call", context, module),
        *onRetFunc = getVoidFunc("tau_prof_func_ret", context, module);

      std::vector<CallInst*> calls;
      for (auto &block : func) {
        for (auto &inst : block) {
          // Only care if it's a function call
          if(auto *op = dyn_cast<CallInst>(&inst)) {
            calls.push_back(op);
          }
        }
      }

      for(auto *op : calls) {
        Function *callee = op->getCalledFunction();
        StringRef calleeName = callee->getName();
        IRBuilder<> builder(op);
        Value *strArg = builder.CreateGlobalStringPtr(calleeName);
        errs() << "I saw a function called " << calleeName << '\n'
               << "The type of the constant string is " << *strArg->getType() << '\n';



        std::vector<Value *> args{strArg};
        builder.CreateCall(onCallFunc, args);

        // Insert the onRetFunc *after* the current instruction
        builder.SetInsertPoint(op->getParent(), ++builder.GetInsertPoint());
        builder.CreateCall(onRetFunc, args);

        mutated = true;
      }

      return mutated;
    }

    // Note: override getAnalysisUsage to specify if any invariants are held over the pass
    // e.g. no mutation of CFG
  };
}

char Instrument::ID = 0;

// Used by the LLVM Guide
//static RegisterPass<Instrument> X("Instrument", "Instrumentation Pass");

// Used by Adrian Sampson

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerInstrumentPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new Instrument());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerInstrumentPass);
