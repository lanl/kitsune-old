//===- RealmABI.cpp - Lower Tapir into Realm runtime system calls -----------===//
`//
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

#if 0
// ### From Legion
typedef Realm::ReductionOpID ReductionOpID;
typedef Realm::ReductionOpUntyped ReductionOpUntyped;
typedef Realm::CustomSerdezID CustomSerdezID;
typedef Realm::CustomSerdezUntyped CustomSerdezUntyped;
typedef Realm::Event Event;
typedef Realm::UserEvent UserEvent;
typedef Realm::Barrier Barrier;
typedef Realm::Reservation Reservation;
typedef Realm::Processor Processor;
typedef Realm::Memory Memory;
typedef Realm::RegionInstance RegionInstance;
typedef Realm::Machine Machine;
typedef Realm::Runtime Runtime;
typedef ::realm_id_t IDType;
// ### end From Legion
#endif
typedef Realm::Processor::TaskFuncPtr TaskFuncPtr

#if 0
// ### Qthreads typedefs
typedef uint64_t aligned_t; 
typedef uint8_t sync_t; 
typedef aligned_t (*qthread_f)(void* arg); 
typedef int (qthread_initialize_t)(); 
typedef unsigned short (qthread_num_workers_t)(); 
typedef int (qthread_fork_copyargs_t)(qthread_f f, const void *arg, size_t arg_size, aligned_t *ret); 
typedef sync_t* (qt_sinc_create_t)(size_t size, void* initval, void* op, size_t expect);  
typedef void (qt_sinc_expect_t)(sync_t* s, size_t incr); 
typedef void (qt_sinc_submit_t)(sync_t* s, void* val); 
typedef void (qt_sinc_wait_t)(sync_t* s, void* target); 
typedef void (qt_sinc_destroy_t)(sync_t* s);  
// ### end Qthreads typedefs
#endif

//########## Begin all my Realm wrapper stuff #########
#include "realm.h"
#include <set>
#include <vector>

void realmSpawn(Realm::Processor::TaskFuncPtr func, const void* args, size_t arglen, void* user_data, size_t user_data_len);

#if 0
extern "C" {
  
  typedef struct context {
    Realm::Runtime rt;
    std::set<Realm::Event> events;
    std::set<Realm::Event> mem_events;
    unsigned cur_task;
    //Realm::ProfilingRequestSet prs; //the default profiling request set
  } context;

  static context *_globalCTX;  //global variable
  
  context * getRealmCTX() {
    //std::cout << "start of getRealmCTX" << std::endl;
    if ( _globalCTX) {
      //std::cout << "_globalCTX exists : returning it" << std::endl;
      return _globalCTX;
    }
    else {
      //std::cout << "_globalCTX does not exist: returning a null pointer" << std::endl;
      return NULL;
    }
  }
  
  void realmInitRuntime(int argc, char** argv) {
    std::cout << "start of realmInitRuntime" << std::endl;

    _globalCTX = new context();
    std::cout << "context created" << std::endl;

    _globalCTX->rt = Realm::Runtime();
    std::cout << "runtime object created" << std::endl;
    _globalCTX->rt.init(&argc, &argv);
    std::cout << "Runtime initialized" << std::endl;
    
    _globalCTX->cur_task = Realm::Processor::TASK_ID_FIRST_AVAILABLE;
    std::cout << "curtask set" << std::endl;
    
    std::cout << "set _globalCTX - All Done" << std::endl;

    return;
  }

  //helper - makes a Realm::Processor::TaskFuncPtr out of available things
  void canonicalTask(const void *args, size_t arglen, void *user_data, size_t user_data_len, Realm::Processor p);
  void canonicalTask(const void *args, size_t arglen, void *user_data, size_t user_data_len, Realm::Processor p){}
  Realm::Processor::TaskFuncPtr makeCanonicalTask(const void *args, size_t arglen, void *user_data, size_t user_data_len, Realm::Processor p);
  makeCanonicalTask(const void *args, size_t arglen, void *user_data, size_t user_data_len, Realm::Processor p){
    Realm::Processor::TaskFuncPtr task = 
      return task;
  }

  //NOTE: this is for integers for now
  //void realmSpawn(void (*func)(void), 
  void realmSpawn(const char *lib_name, const char *symbol_name, 
		  const void* args, 
		  size_t arglen, 
		  void* user_data, 
		  size_t user_data_len){ 
    //		  void* data_region) {           
    /* take a function pointer to the task you want to run, 
       creates a CodeDescriptor from it
       needs pointer to user data and arguments (NULL for void?)
       needs size_t for len (0 for void?)
       data_region is actually a pointer to a RegionInstance
     */
    std::cout << "start of realmSpawn" << std::endl;
    context *ctx = getRealmCTX();
    std::cout << "successfully got the realm context" << std::endl;

    //update current taskID
    ctx->cur_task++;
    std::cout << "updated cur_task" << std::endl;
    Realm::Processor::TaskFuncID taskID = ctx->cur_task;
    std::cout << "created the taskID" << std::endl;

    //get a processor to run on
    Realm::Machine::ProcessorQuery procquery(Realm::Machine::get_machine());
    Realm::Processor p = procquery.local_address_space().random();
    assert ( p != Realm::Processor::NO_PROC); //assert that the processor exists

    //get a memory associated with that processor to copy to
    //Realm::Machine::MemoryQuery memquery(Realm::Machine::get_machine());
    //Realm::Memory m = memquery.local_address_space().best_affinity_to(p).random();
    //assert ( m != Realm::Memory::NO_MEMORY); //assert that the memory exists

    //takes fxn ptr, turns it into a TypeConv::from_cpp_type<TaskFuncPtr>()
    // the CodeDescriptor needs to be of that type

    // orig:     Realm::CodeDescriptor cd = Realm::CodeDescriptor(func);
    //old:Realm::CodeDescriptor cd = Realm::CodeDescriptor(Realm::TypeConv::from_cpp_type<Realm::Processor::TaskFuncPtr>());
    
/////////HERE IS WHERE YOU WERE WORKING
    canonicalTask(args, arglen, user_data, user_data_len, p);
    Realm::CodeDescriptor cd = Realm::CodeDescriptor(canonicalTask);
    std::cout << "Created CodeDescriptor" << std::endl;
    //Realm::FunctionPointerImplementation fpi = Realm::FunctionPointerImplementation(func);
    //std::cout << "Created FunctionPointerImplementation" << std::endl;
    Realm::DSOReferenceImplementation fpi = Realm::DSOReferenceImplementation(std::string(lib_name), std::string(symbol_name));
    cd.add_implementation(&fpi);
    std::cout << "added the implementation to the CodeDescriptor" << std::endl;

    const Realm::ProfilingRequestSet prs;  //We don't care what it is for now, the default is fine
    std::cout << "Created a default ProfilingRequestSet" << std::endl;

    //register the task with the runtime
    Realm::Event e1 = p.register_task(taskID, cd, prs, user_data, user_data_len);
    std::cout << "Registered Task" << std::endl;
    ctx->events.insert(e1); //might not actually need to keep track of this one
    std::cout << "Added the register_task event to the context's set of Events" << std::endl;

    //spawn the task
    Realm::Event e2 = p.spawn(taskID, args, arglen, mem_sync(), 0); //predicated on the creation and initialization of the region 
    std::cout << "Spawned the task" << std::endl;
    ctx->events.insert(e2);
    std::cout << "Added the spawn event to the context's set of Events" << std::endl;

    return;
  }
  
  void realmSync() {
    std::cout << "Start of realmSync" << std::endl;
    context *ctx = getRealmCTX();
    Realm::Event e;
    e = e.merge_events(ctx->events);
    std::cout << "merged events" << std::endl;

    ctx->events.clear();
    std::cout << "Cleared the context's set of events" << std::endl;
    ctx->events.insert(e);
    std::cout << "Added sync event to context's set of events" << std::endl;

    while (! e.has_triggered()) {
      std::cout << "Sync event NOT TRIGGERED" << std::endl;
      continue;
    }
    std::cout << "Sync event TRIGGERED" << std::endl;

    return;
  }
}

//########## End all my Realm wrapper stuff #########
#endif

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
  
} // namespace
#endif
//##############################################################################

RealmABI::RealmABI() { }
RealmABI::~RealmABI() { }

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
  builder.CreateCall(getFunction<>(M,"realmSync"), args, "realmSync");

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
  ArrayRef<Type*> FnParams = extracted->getFunctionType()->params();
  StructType *ArgsTy = StructType::create(FnParams, "anon", true); //isPacked
  auto *ArgsPtrTy = PointerType::getUnqual(ArgsTy);

  //get the argument data
  std::vector<Value *> FnData;
  for (auto& arg : extracted->args()) {
    FnData.push_back(&arg);
  }
  StructType *ArgsData = StructType::create(FnData, "anon2", true); //isPacked
  auto *ArgsPtrData = PointerType::getUnqual(ArgsData);

  //get a processor to run on
  Realm::Machine::ProcessorQuery procquery(Realm::Machine::get_machine());
  Realm::Processor p = procquery.local_address_space().random();
  assert ( p != Realm::Processor::NO_PROC); //assert that the processor exists

  //Create the canonical TaskFuncPtr
  FunctionType *OutlinedFnTy = FunctionType::get(
      Type::getVoidTy(C), 
      {ArgsPtrTy, ArgsTy->getNumElements(), ArgsPtrData, DL.getTypeAllocSize(ArgsData), p},
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

  for (auto& ret : retinsts) {
    //qthread way: auto retzero = ReturnInst::Create(C, ConstantInt::get(Type::getInt64Ty(C), 0));
    auto retvoid = ReturnInst::CreateRetVoid();
    ReplaceInstWithInst(ret, retvoid);
  }

  // Caller code
  auto callerArgStruct = CallerIRBuilder.CreateAlloca(ArgsTy); 
  auto callerDataStruct = CallerIRBuilder.CreateAlloca(ArgsData); 
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
  auto argSize = ConstantInt::get(Type::getInt64Ty(C), DL.getTypeAllocSize(ArgsTy)); 
  auto argDataSize = ConstantInt::get(Type::getInt64Ty(C), DL.getTypeAllocSize(ArgsData)); 
  auto null = Constant::getNullValue(Type::getInt64PtrTy(C)); 
  auto argsStructVoidPtr = CallerIRBuilder.CreateBitCast(callerArgStruct, Type::getInt8PtrTy(C)); 
  auto argsDataVoidPtr = CallerIRBuilder.CreateBitCast(callerDataStruct, Type::getInt8PtrTy(C)); 
  std::vector<Value *> callerArgs = { outlinedFnPtr, argsStructVoidPtr, argSize, argsDataVoidPtr, argDataSize}; 
  CallerIRBuilder.CreateCall(getFunction(M, "realmSpawn"), callerArgs);

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
  Module *M = F->getParent(); 
  LLVMContext& C = M->getContext(); 
  IRBuilder<> B(C);

  //get argc and argv - put them in an ArrayRef<Value*>
  //I think a std::vector<Value*> would also work here instead of the ArrayRef
  ArrayRef<Value*> args (F.arg_begin(),F.arg_end());


  //CallInst::Create(get_qthread_initialize(*F.getParent()), "", F.getEntryBlock().getFirstNonPHIOrDbg());
  //CallInst::Create(get_realmInitRuntime(*F.getParent()), "", F.getEntryBlock().getFirstNonPHIOrDbg());

  //OR
  B.createCall(getFunction<>(M,"realmInitRuntime"), args, "realmInitRuntime")
  return true;
}

