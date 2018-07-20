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


#include <fstream>
#include <regex>

#include "llvm/Pass.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

// Need these to use Sampson's registration technique
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"

#ifdef TAU_PROF_CXX
#include <cxxabi.h>
#endif

// Other passes do this, so I assume the macro is useful somewhere
#define DEBUG_TYPE "tau-profile"


using namespace llvm;

namespace {

// Command line options for this plugin.  These permit the user to specify what
// functions should be instrumented and what profiling functions to call.  The
// only real caveat is that the profiling function symbols must be present in
// some source/object/library or compilation will fail at link-time.
cl::opt<std::string>
TauInputFile("tau-input-file",
             cl::desc("Specify file containing the names of functions to instrument"),
             cl::value_desc("filename"));

cl::opt<std::string>
TauStartFunc("tau-start-func",
             cl::desc("Specify the profiling function to call before functions of interest"),
             cl::value_desc("Function name"),
             cl::init("Tau_start"));

cl::opt<std::string>
TauStopFunc("tau-stop-func",
            cl::desc("Specify the profiling function to call after functions of interest"),
            cl::value_desc("Function name"),
            cl::init("Tau_stop"));

cl::opt<std::string>
TauRegex("tau-regex",
         cl::desc("Specify a regex to identify functions interest (case-sensitive)"),
         cl::value_desc("Regular Expression"),
         cl::init(""));

cl::opt<std::string>
TauIRegex("tau-iregex",
         cl::desc("Specify a regex to identify functions interest (case-insensitive)"),
         cl::value_desc("Regular Expression"),
         cl::init(""));

cl::opt<bool>
TauDryRun("tau-dry-run",
         cl::desc("Don't actually instrument the code, just print what would be instrumented"));



// Demangling technique borrowed/modified from
// https://github.com/eklitzke/demangle/blob/master/src/demangle.cc
static StringRef normalize_name(StringRef mangled_name) {
#ifdef TAU_PROF_CXX
  int status = 0;

  const char *str = abi::__cxa_demangle(mangled_name.begin(), 0, 0, &status);
  StringRef realname{str};

  switch (status) {
  case 0:
    break;
  case -1:
    // errs() << "FAIL: failed to allocate memory while demangling "
    //        << mangled_name << '\n';
    break;
  case -2:
    // errs() << "FAIL: " << mangled_name
    //        << " is not a valid name under the C++ ABI mangling rules\n";
    break;
  default:
    // errs() << "FAIL: couldn't demangle " << mangled_name
    //        << " for some unknown reason: " << status << '\n';
    break;
  }

  return realname;
#else
  return mangled_name;
#endif
}

  /*!
   *  Find/declare a function taking a single `i8*` argument with a void return
   *  type suitable for making a call to in IR. This is used to get references
   *  to the TAU profiling function symbols.
   *
   * \param funcname The name of the function
   * \param ctx The LLVMContext
   * \param mdl The Module in which the function will be used
   */
  static Constant *getVoidFunc(StringRef funcname, LLVMContext &context, Module *module) {

    // Void return type
    Type *retTy = Type::getVoidTy(context);

    // single i8* argument type (char *)
    Type *argTy = Type::getInt8PtrTy(context);
    SmallVector<Type *, 1> paramTys{argTy};

    // Third param to `get` is `isVarArg`.  It's not documented, but might have
    // to do with variadic functions?
    FunctionType *funcTy = FunctionType::get(retTy, paramTys, false);
    return module->getOrInsertFunction(funcname, funcTy);
  }


  /*!
   * The instrumentation pass.
   */
  struct Instrument : public FunctionPass {

    using CallAndName = std::pair<CallInst *, StringRef>;

    static char ID; // Pass identification, replacement for typeid
    StringSet<> funcsOfInterest;

    // basic ==> POSIX regular expression
    std::regex rex{TauRegex,
                   std::regex_constants::ECMAScript};
    std::regex irex{TauIRegex,
                    std::regex_constants::ECMAScript | std::regex_constants::icase};

    Instrument() : FunctionPass(ID) {
      if(!TauInputFile.empty()) {
        std::ifstream ifile{TauInputFile};
        loadFunctionsFromFile(ifile);
      }
    }

    /*!
     *  Given an open file, read each line as the name of a function that should
     *  be instrumented.  This modifies the class member funcsOfInterest to hold
     *  strings from the file.
     */
    void loadFunctionsFromFile(std::ifstream & file) {
      std::string funcName;
      while(std::getline(file, funcName)) {
        errs() << "registered '" << funcName << "' for profiling\n";
        funcsOfInterest.insert(funcName);
      }
    }


    /*!
     *  The FunctionPass interface method, called on each function produced from
     *  the original source.
     */
    bool runOnFunction(Function &func) override {
      // Surely *most* functions make fewer than 50 calls to other functions
      // that we want to instrument
      SmallVector<CallAndName, 50> calls;

      for (auto &block : func) {
        for (auto &inst : block) {
          if (auto *op = dyn_cast<CallInst>(&inst)) {
            // Inserting new function calls here (via IRBuilder, e.g.) will
            // modify the block and potentially give us a never-ending list of
            // calls to instrument.  It's simpler to just gather them up and
            // mess with them afterwards.
            maybeSaveForProfiling(op, calls);
          }
        }
      }
      if(TauDryRun && calls.size() > 0) {

        // TODO: Fix this.
        // getName() doesn't seem to give a properly mangled name
        auto pretty_name = normalize_name(func.getName());
        if(pretty_name.empty()) pretty_name = func.getName();

        errs() << "In function " << pretty_name
               << "\nThe following would be instrumented:\n";
        for (auto &pair : calls) {
          errs() <<  pair.second << '\n';
        }
        errs() << '\n';
        return false; // Dry run does not modify anything
      }
      return addInstrumentation(calls, func);
    }

    /*!
     *  Inspect the given CallInst and, if it should be profiled, add it
     *  and its recognized name the given vector.
     *
     * \param call The CallInst to inspect
     * \param calls Vector to add to, if the CallInst should be profiled
     */
    void maybeSaveForProfiling(CallInst *call, SmallVectorImpl<CallAndName> &calls) {
      if(auto *callee = call->getCalledFunction()) {
        StringRef calleeName = normalize_name(callee->getName());
        if(calleeName.empty()) {
          // We may still want to instrument a call with an unmangled name.
          // Assume any failed demangling won't accidentally leave us with a
          // misleading name.
          calleeName = callee->getName();
        }

        if(funcsOfInterest.count(calleeName) > 0 || regexFits(calleeName)) {
          calls.push_back({call, calleeName});
        }
      }
    }

    // TODO: Doc
    bool regexFits(const StringRef & name) {
      bool match = false, imatch = false;
      if(!TauRegex.empty()) match = std::regex_search(name.str(), rex);
      if(!TauIRegex.empty()) imatch = std::regex_search(name.str(), irex);
      return match || imatch;
    }


    /*!
     *  Add instrumentation to the CallInsts in the given vector, using the
     *  given function for context.
     *
     * \param calls vector of calls to add profiling to
     * \param func Function in which the calls were found
     * \return False if no new instructions were added (only when calls is empty),
     *  True otherwise
     */
    bool addInstrumentation(SmallVectorImpl<CallAndName> &calls, Function &func) {

      // Declare and get handles to the runtime profiling functions
      auto &context = func.getContext();
      auto *module = func.getParent();
      Constant
        *onCallFunc = getVoidFunc(TauStartFunc, context, module),
        *onRetFunc = getVoidFunc(TauStopFunc, context, module);

      bool mutated = false;
      for (auto &pair : calls) {
        auto *op = pair.first;
        auto calleeName = pair.second;
        IRBuilder<> builder(op);

        // This is the recommended way of creating a string constant (to be used
        // as an argument to runtime functions)
        Value *strArg = builder.CreateGlobalStringPtr(calleeName);
        SmallVector<Value *, 1> args{strArg};

        // Before the CallInst
        builder.CreateCall(onCallFunc, args);

        // Set insert point to just after the CallInst
        builder.SetInsertPoint(op->getParent(), ++builder.GetInsertPoint());
        builder.CreateCall(onRetFunc, args);

        mutated = true;
      }
      return mutated;
    }
  };
}

char Instrument::ID = 0;

static RegisterPass<Instrument> X("tau-prof", "TAU Profiling", false, false);

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerInstrumentPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new Instrument());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerInstrumentPass);
