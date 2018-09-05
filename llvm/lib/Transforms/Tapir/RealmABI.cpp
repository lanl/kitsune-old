//===- RealmABI.cpp - Lower Tapir into Realm runtime system calls -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the RealmABI interface, which is used to convert Tapir
// instructions -- detach, reattach, and sync -- to calls into the Realm
// runtime system.  
//
//===----------------------------------------------------------------------===//

#define MYDEBUG true
#if MYDEBUG
#include <iostream>
#endif

#include "llvm/Transforms/Tapir/RealmABI.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Transforms/Tapir/Outline.h"
#include "llvm/Transforms/Utils/EscapeEnumerator.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/TapirUtils.h"

using namespace llvm;

#define DEBUG_TYPE "realmabi"
typedef void (*TaskFuncPtr)(const void *args, size_t arglen,
			    const void *user_data, size_t user_data_len,
			    unsigned long long);

RealmABI::RealmABI() { }
RealmABI::~RealmABI() { }

Value *RealmABI::GetOrCreateWorker8(Function &F) {
  LLVMContext& C = F.getParent()->getContext(); 
  return ConstantInt::get(C, APInt(16, 8)); //Note: stole this from PTXABI.cpp
}

void RealmABI::createSync(SyncInst &SI, ValueToValueMapTy &DetachCtxToStackFrame) {
  IRBuilder<> builder(&SI); 
  auto F = SI.getParent()->getParent(); 
  auto M = F->getParent();
  auto& C = M->getContext(); 

  std::vector<Value*> args; //empty because realmSync takes no arguments
  FunctionType * Fty = FunctionType::get(Type::getVoidTy(C), Type::getVoidTy(C), false);
  Function * thisFunc = Function::Create(Fty, GlobalValue::ExternalLinkage, "realmSync", M);
  //std::cout << "args size: " << args.size() << std::endl;
  //std::cout << "thisFunc arg size: " << thisFunc->arg_size() << std::endl;
  //std::cout << "thisFunc num params: " << thisFunc->getFunctionType()->getNumParams() << std::endl;

  CallInst::Create(Fty, thisFunc, args, "", F->getEntryBlock().getFirstNonPHIOrDbg());

  BranchInst *PostSync = BranchInst::Create(SI.getSuccessor(0));
  ReplaceInstWithInst(&SI, PostSync);
  return;
}

// Adds entry basic blocks to body of extracted, replacing extracted, and adds
// necessary code to call, i.e. storing arguments in struct
Function* formatFunctionToRealmF(Function* extracted, CallInst* cal){
  std::vector<Value*> LoadedCapturedArgs;
  for(auto& a:cal->arg_operands()) {
    LoadedCapturedArgs.push_back(a);
  }

  Module *M = extracted->getParent(); 
  auto& C = M->getContext(); 
  DataLayout DL(M);
  IRBuilder<> CallerIRBuilder(cal);

  //get the argument types
  auto FnParams = extracted->getFunctionType()->params();
  StructType *ArgsTy = StructType::create(FnParams, "anon");
  auto *ArgsPtrTy = PointerType::getUnqual(ArgsTy);

  //Create the canonical TaskFuncPtr
  ArrayRef<Type*> typeArray = {ArgsPtrTy, Type::getInt64Ty(C), ArgsPtrTy, Type::getInt64Ty(C), Type::getInt64Ty(C)}; 
  //trying int64 as stand-in for Realm::Processor because a ::realm_id_t is ultimately and unsigned long long

  FunctionType *OutlinedFnTy = FunctionType::get(
      Type::getVoidTy(C), 
      typeArray,
      false);

  Function *OutlinedFn = Function::Create(
      OutlinedFnTy, GlobalValue::InternalLinkage, ".realm_outlined.", M);
  OutlinedFn->addFnAttr(Attribute::AlwaysInline);
  OutlinedFn->addFnAttr(Attribute::NoUnwind);
  OutlinedFn->addFnAttr(Attribute::UWTable);

  StringRef ArgNames[] = {".args"};
  std::vector<Value*> out_args;
  for (auto &Arg : OutlinedFn->args()) {
    Arg.setName(ArgNames[out_args.size()]);
    out_args.push_back(&Arg);
  }

  // Entry Code
  auto *EntryBB = BasicBlock::Create(C, "entry", OutlinedFn, nullptr);
  IRBuilder<> EntryBuilder(EntryBB);
  auto argStructPtr = EntryBuilder.CreateBitCast(out_args[0], ArgsPtrTy); 
  ValueToValueMapTy valmap;

  unsigned int argc = 0;
  for (auto& arg : extracted->args()) {
    auto *DataAddrEP = EntryBuilder.CreateStructGEP(ArgsTy, argStructPtr, argc); 
    auto *DataAddr = EntryBuilder.CreateAlignedLoad(
        DataAddrEP,
        DL.getTypeAllocSize(DataAddrEP->getType()->getPointerElementType()));
    valmap.insert(std::pair<Value*,Value*>(&arg,DataAddr));
    argc++;
  }

  // Replace return values with return void
  SmallVector< ReturnInst *,5> retinsts;
  CloneFunctionInto(OutlinedFn, extracted, valmap, true, retinsts);
  EntryBuilder.CreateBr(OutlinedFn->getBasicBlockList().getNextNode(*EntryBB));

  Value * dummyVal = nullptr;
  Instruction * dummyInst = nullptr;
  for (auto& ret : retinsts) {
    auto retvoid = ReturnInst::Create(C, dummyVal, dummyInst);
    ReplaceInstWithInst(ret, retvoid);
  }

  // Caller code
  auto callerArgStruct = CallerIRBuilder.CreateAlloca(ArgsTy); 

  unsigned int cArgc = 0;
  for (auto& arg : LoadedCapturedArgs) {
    auto *DataAddrEP2 = CallerIRBuilder.CreateStructGEP(ArgsTy, callerArgStruct, cArgc); 
    CallerIRBuilder.CreateAlignedStore(
        LoadedCapturedArgs[cArgc], DataAddrEP2,
        DL.getTypeAllocSize(arg->getType()));
    cArgc++;
  }

  assert(argc == cArgc && "Wrong number of arguments passed to outlined function"); 
  //assert(argc == ArgsTy->getNumElements() && "Wrong number of arguments passed to outlined function"); 

  auto outlinedFnPtr = CallerIRBuilder.CreatePointerBitCastOrAddrSpaceCast(
									   OutlinedFn, TypeBuilder<TaskFuncPtr, false>::get(M->getContext())); 
  auto argSize = ConstantInt::get(Type::getInt64Ty(C), ArgsTy->getNumElements()); 
  auto argDataSize = ConstantInt::get(Type::getInt64Ty(C), DL.getTypeAllocSize(ArgsTy)); 

  auto argsStructVoidPtr = CallerIRBuilder.CreateBitCast(callerArgStruct, Type::getInt8PtrTy(C)); 

  std::vector<Value *> callerArgs = { outlinedFnPtr, argsStructVoidPtr, argSize, argsStructVoidPtr, argDataSize}; 

  FunctionType * TaskFuncPtrTy = FunctionType::get(Type::getVoidTy(C),
	       {PointerType::getUnqual(Type::getVoidTy(C)),
		Type::getInt64Ty(C),
		PointerType::getUnqual(Type::getVoidTy(C)),
		Type::getInt64Ty(C),
		Type::getInt64Ty(C)}, //end argument list
		false);
  FunctionType * Fty = FunctionType::get(Type::getVoidTy(C), 
	       {TaskFuncPtrTy,
		PointerType::getUnqual(Type::getVoidTy(C)),
		Type::getInt64Ty(C),
		PointerType::getUnqual(Type::getVoidTy(C)),
		Type::getInt64Ty(C),
		Type::getInt64Ty(C)}, //end argument list
		false);
  Function * thisFunc = Function::Create(Fty, GlobalValue::ExternalLinkage, "realmSync", M);
  //std::cout << "args size: " << callerArgs.size() << std::endl;
  //std::cout << "thisFunc arg size: " << thisFunc->arg_size() << std::endl;
  //std::cout << "thisFunc num params: " << thisFunc->getFunctionType()->getNumParams() << std::endl;

  CallInst::Create(Fty, thisFunc, callerArgs, "", extracted->getEntryBlock().getFirstNonPHIOrDbg());

  cal->eraseFromParent();
  extracted->eraseFromParent();

  DEBUG(OutlinedFn->dump()); 

  return OutlinedFn; 
}

Function *RealmABI::createDetach(DetachInst &detach,
				    ValueToValueMapTy &DetachCtxToStackFrame,
				    DominatorTree &DT, AssumptionCache &AC) {
  BasicBlock *detB = detach.getParent();
  Function &F = *(detB->getParent());
  BasicBlock *Spawned  = detach.getDetached();
  BasicBlock *Continue = detach.getContinue();

  CallInst *cal = nullptr;
  Function *extracted = extractDetachBodyToFunction(detach, DT, AC, &cal);
  extracted = formatFunctionToRealmF(extracted, cal); 

  // Replace the detach with a branch to the continuation.
  BranchInst *ContinueBr = BranchInst::Create(Continue);
  ReplaceInstWithInst(&detach, ContinueBr);

  // Rewrite phis in the detached block.
  {
    BasicBlock::iterator BI = Spawned->begin();
    while (PHINode *P = dyn_cast<PHINode>(BI)) {
      P->removeIncomingValue(detB);
      ++BI;
    }
  }

  DEBUG(F.dump()); 

  return extracted;
}

void RealmABI::preProcessFunction(Function &F) {}

void RealmABI::postProcessFunction(Function &F) {}

void RealmABI::postProcessHelper(Function &F) {}

bool RealmABI::processMain(Function &F) {
  Module *M = F.getParent(); 
  LLVMContext& C = M->getContext(); 
  IRBuilder<> B(C);

  //get argc and argv 
  //auto argTypes = F.getFunctionType()->params();

  std::vector<Value*> args;

  #if 0
  ValueSymbolTable *symtab = F.getValueSymbolTable();
  std::cout << "got symbol table" << std::endl;
  if (symtab != nullptr) {
    args.push_back(symtab->lookup("argc"));
    args.push_back(symtab->lookup("argv"));
    std::cout << "finished symbol table lookup" << std::endl;
  }
  else
    std::cout << "symbol table is a nullptr" << std::endl;
  #endif

  for (auto arg = F.arg_begin(); arg < F.arg_end(); arg++) {
    args.push_back(arg);
  }

  //This is the real signature of realmInitRuntime, but there's a problem with voids
  //FunctionType * Fty = FunctionType::get(Type::getVoidTy(C), {Type::getInt32Ty(C), PointerType::getUnqual(Type::getInt8PtrTy(C))}, false);
  FunctionType * Fty = FunctionType::get(Type::getInt32Ty(C), {Type::getInt32Ty(C), PointerType::getUnqual(Type::getInt8PtrTy(C))}, false);

  Function * thisFunc = Function::Create(Fty, GlobalValue::ExternalLinkage, "realmInitRuntime", M);
  std::cout << "args size: " << args.size() << std::endl;
  std::cout << "thisFunc arg size: " << thisFunc->arg_size() << std::endl;
  std::cout << "thisFunc num params: " << thisFunc->getFunctionType()->getNumParams() << std::endl;

  CallInst::Create(Fty, thisFunc, args, "", F.getEntryBlock().getFirstNonPHIOrDbg());
  return true;
}
