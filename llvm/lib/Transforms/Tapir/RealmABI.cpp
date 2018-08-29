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
			    const void *user_data, size_t user_data_len);
                            //Processor proc);
//better idea, borrowed from Nick's PTXABI.cpp
namespace{

  //this is used for calling the Realm runtime wrapper functions directly by name
  template<class F>
  Function* getFunction(Module& M, const char* name){
    return cast<Function>(M.getOrInsertFunction(name,
      TypeBuilder<F, false>::get(M.getContext())));
  } 
  //I don't know if I actually need this
#if 0
  template<class B>
  Value* convertInteger(B& b, Value* from, Value* to, const std::string& name){
    auto ft = dyn_cast<IntegerType>(from->getType());
    assert(ft && "expected from type as integer type");

    auto tt = dyn_cast<IntegerType>(to->getType());
    assert(tt && "expected to type as integer type");

    if(ft->getBitWidth() > tt->getBitWidth()){
      return b.CreateTrunc(from, tt, name);
    }
    else if(ft->getBitWidth() < tt->getBitWidth()){
      return b.CreateZExt(from, tt, name);
    }

    return from;
  }
#endif
  
} // namespace
//##############################################################################

RealmABI::RealmABI() { }
RealmABI::~RealmABI() { }

Value *RealmABI::GetOrCreateWorker8(Function &F) {
  LLVMContext& C = F.getParent()->getContext(); 
  Value* null = Constant::getNullValue(Type::getInt8PtrTy(C)); 
  return null;
}

#if 0
static const StringRef worker8_name = "qthread_nworker8";

/// \brief Get/Create the worker count for the spawning function. We stick it
// at the end of the entry block to ensure that if we are in main it occurs
// after initialization
Value *RealmABI::GetOrCreateWorker8(Function &F) {
  Value *P0 = CallInst::Create(REALM_FUNC(qthread_num_workers, *F.getParent()), "", F.getEntryBlock().getTerminator());
  Value *P8 = BinaryOperator::Create(Instruction::Mul, P0, ConstantInt::get(P0->getType(), 8), worker8_name, F.getEntryBlock().getTerminator());
  return P8;
}

Value* getOrCreateSinc(ValueToValueMapTy &valmap, Value* SyncRegion, Function *F){
  Module *M = F->getParent(); 
  LLVMContext& C = M->getContext(); 
  Value* sinc; 
  if((sinc = valmap[SyncRegion]))
    return sinc;
  else {
    Value* zero = ConstantInt::get(Type::getInt64Ty(C), 0); 
    Value* null = Constant::getNullValue(Type::getInt8PtrTy(C)); 
    std::vector<Value*> createArgs = {zero, null, null, zero}; 
    sinc = CallInst::Create(REALM_FUNC(qt_sinc_create, *M), createArgs, "",  
                         F->getEntryBlock().getTerminator()); 
    valmap[SyncRegion] = sinc;

    // Make sure we destroy the sinc at all exit points to prevent memory leaks
    for(BasicBlock &BB : *F){
      if(isa<ReturnInst>(BB.getTerminator())){
        CallInst::Create(REALM_FUNC(qt_sinc_destroy, *M), {sinc}, "", BB.getTerminator()); 
      }
    }

    return sinc; 
  }
}
#endif
void RealmABI::createSync(SyncInst &SI, ValueToValueMapTy &DetachCtxToStackFrame) {
  IRBuilder<> builder(&SI); 
  auto F = SI.getParent()->getParent(); 
  auto M = F->getParent();
  auto& C = M->getContext(); 
  //auto null = Constant::getNullValue(Type::getInt8PtrTy(C)); 
  //Value* SR = SI.getSyncRegion(); 
  //auto sinc = getOrCreateSinc(DetachCtxToStackFrame, SR, F); 
  //std::vector<Value *> args = {sinc, null}; 
  //auto sincwait = REALM_FUNC(qt_sinc_wait, *M); 
  //builder.CreateCall(sincwait, args);

  ArrayRef<Value*> args; //empty because realmSync takes no arguments
  builder.CreateCall(::getFunction<void(*)(void)>(*M,"realmSync"), args, "realmSync");

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
  //StructType *ArgsTy = StructType::create(FnParams, "anon", true); //isPacked
  auto *ArgsPtrTy = PointerType::getUnqual(ArgsTy);

  //get the argument data
  //std::vector<Value *> FnData;
  //for (auto& arg : extracted->args()) {
  //FnData.push_back(&arg);
  //}
  //auto FnData = extracted->args();
  //StructType *ArgsData = StructType::create(FnData, "anon2");
  //StructType *ArgsData = StructType::create(FnData, "anon2", true); //isPacked
  //auto *ArgsPtrData = PointerType::getUnqual(ArgsData);

  //get a processor to run on
  //Realm::Machine::ProcessorQuery procquery(Realm::Machine::get_machine());
  //Realm::Processor p = procquery.local_address_space().random();
  //assert ( p != Realm::Processor::NO_PROC); //assert that the processor exists

  //Create the canonical TaskFuncPtr
  ArrayRef<Type*> typeArray = {ArgsPtrTy, Type::getInt128Ty(C), ArgsPtrTy, Type::getInt128Ty(C)};
  FunctionType *OutlinedFnTy = FunctionType::get(
      Type::getVoidTy(C), 
      //{ArgsPtrTy, ArgsTy->getNumElements(), ArgsPtrData, DL.getTypeAllocSize(ArgsData)},
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
    //qthread way: auto retzero = ReturnInst::Create(C, ConstantInt::get(Type::getInt64Ty(C), 0));
    auto retvoid = ReturnInst::Create(C, dummyVal, dummyInst);
    ReplaceInstWithInst(ret, retvoid);
  }

  // Caller code
  auto callerArgStruct = CallerIRBuilder.CreateAlloca(ArgsTy); 
  //auto callerDataStruct = CallerIRBuilder.CreateAlloca(ArgsData); 
  unsigned int cArgc = 0;
  for (auto& arg : LoadedCapturedArgs) {
    auto *DataAddrEP = CallerIRBuilder.CreateStructGEP(ArgsTy, callerArgStruct, cArgc); 
    CallerIRBuilder.CreateAlignedStore(
        LoadedCapturedArgs[cArgc], DataAddrEP,
        DL.getTypeAllocSize(arg->getType()));
    cArgc++;
  }

  assert(argc == cArgc && "Wrong number of arguments passed to outlined function"); 
  //assert(argc == ArgsTy->getNumElements() && "Wrong number of arguments passed to outlined function"); 

  auto outlinedFnPtr = CallerIRBuilder.CreatePointerBitCastOrAddrSpaceCast(
									   OutlinedFn, TypeBuilder<TaskFuncPtr, false>::get(M->getContext())); 
  auto argSize = ConstantInt::get(Type::getInt64Ty(C), ArgsTy->getNumElements()); 
  auto argDataSize = ConstantInt::get(Type::getInt64Ty(C), DL.getTypeAllocSize(ArgsTy)); 
  auto null = Constant::getNullValue(Type::getInt64PtrTy(C)); 
  auto argsStructVoidPtr = CallerIRBuilder.CreateBitCast(callerArgStruct, Type::getInt8PtrTy(C)); 
  //auto argsDataVoidPtr = CallerIRBuilder.CreateBitCast(callerDataStruct, Type::getInt8PtrTy(C)); 
  std::vector<Value *> callerArgs = { outlinedFnPtr, argsStructVoidPtr, argSize, argsStructVoidPtr, argDataSize}; 
  CallerIRBuilder.CreateCall(::getFunction<void(*)(TaskFuncPtr,const void*, size_t, void*, size_t)>(*M, "realmSpawn"), callerArgs); //processor?

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

  Module *M = F.getParent();
  LLVMContext &C = M->getContext(); 

  // Get qthreads sinc value
  //Value* SR = detach.getSyncRegion(); 
  //Value* sinc = getOrCreateSinc(DetachCtxToStackFrame, SR, &F);
  
  // Add an expect increment before spawning
  //IRBuilder<> preSpawnB(detB); 
  //Value* one = ConstantInt::get(Type::getInt64Ty(C), 1); 
  //std::vector<Value*> expectArgs = {sinc, one}; 
  //CallInst::Create(REALM_FUNC(qt_sinc_expect, *M), expectArgs, "", &detach); 

  // Add a submit to end of task body
  //IRBuilder<> footerB(Spawned->getTerminator()); 
  //Value* null = Constant::getNullValue(Type::getInt8PtrTy(C)); 
  //std::vector<Value*> submitArgs = {sinc, null}; 
  //footerB.CreateCall(REALM_FUNC(qt_sinc_submit, *M), submitArgs); 

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

  //get argc and argv - put them in an ArrayRef<Value*>
  //I think a std::vector<Value*> would also work here instead of the ArrayRef
  //ArrayRef<Value*> args (F.arg_begin(),F.arg_end());
  auto argTypes = F.getFunctionType()->params();
  std::vector<Value*> args;
  //if (F.arg_empty()) {
  //args.push_back(ConstantInt::get(Type::getInt64Ty(C), 0));
  //args.push_back(ConstantPointerNull::get(PointerType::get(PointerType::getUnqual(Type::getInt64Ty(C))))); //aiming for char** via int**
  //}
  //else {
    //auto argIter = F.arg_begin();
    //assume it is int argc
    //Value *argc = (dyn_cast<ConstantInt>(*argIter))->getValue();
    //argIter++;
    
    //Value argc = F.arg_size();
    //args.push_back(&argc);

    ValueSymbolTable *symtab = F.getValueSymbolTable();
    args.push_back(symtab->lookup("argc"));
    args.push_back(symtab->lookup("argv"));
    //}

  #if 0
  for (auto arg = F.arg_begin(); arg < F.arg_end(); arg++) {
    if(auto* ci = dyn_cast<ConstantInt>(arg))
      args.push_back(ci->getValue());
    else { //array of arrays?
  #endif
  //CallInst::Create(get_qthread_initialize(*F.getParent()), "", F.getEntryBlock().getFirstNonPHIOrDbg());
  //CallInst::Create(get_realmInitRuntime(*F.getParent()), "", F.getEntryBlock().getFirstNonPHIOrDbg());

  //OR
  B.CreateCall(::getFunction< void(*)(int, char**)>(*M,"realmInitRuntime"), args, "realmInitRuntime");
  return true;
}

#if 0
#define REALM_FUNC(name, CGF) get_##name(CGF)

#define DEFAULT_GET_REALM_FUNC(name)                                  \
  static Function *get_##name(Module& M) {         \
    return cast<Function>(M.getOrInsertFunction(            \
                                          #name,            \
                                          TypeBuilder< name ## _t , false >::get(M.getContext()) \
							)); \
  }

// Qthreads - TODO: replace macros with something better
DEFAULT_GET_REALM_FUNC(qthread_num_workers)
DEFAULT_GET_REALM_FUNC(qthread_fork_copyargs)
DEFAULT_GET_REALM_FUNC(qthread_initialize)
DEFAULT_GET_REALM_FUNC(qt_sinc_create)
DEFAULT_GET_REALM_FUNC(qt_sinc_expect)
DEFAULT_GET_REALM_FUNC(qt_sinc_submit)
DEFAULT_GET_REALM_FUNC(qt_sinc_wait)
DEFAULT_GET_REALM_FUNC(qt_sinc_destroy)

// Realm equivalent
DEFAULT_GET_REALM_FUNC(realmInitRuntime)
#endif

