/**
  ***************************************************************************
  * Copyright (c) 2017, Los Alamos National Security, LLC.
  * All rights reserved.
  *
  *  Copyright 2010. Los Alamos National Security, LLC. This software was
  *  produced under U.S. Government contract DE-AC52-06NA25396 for Los
  *  Alamos National Laboratory (LANL), which is operated by Los Alamos
  *  National Security, LLC for the U.S. Department of Energy. The
  *  U.S. Government has rights to use, reproduce, and distribute this
  *  software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY,
  *  LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
  *  FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
  *  derivative works, such modified software should be clearly marked,
  *  so as not to confuse it with the version available from LANL.
  *
  *  Additionally, redistribution and use in source and binary forms,
  *  with or without modification, are permitted provided that the
  *  following conditions are met:
  *
  *    * Redistributions of source code must retain the above copyright
  *      notice, this list of conditions and the following disclaimer.
  *
  *    * Redistributions in binary form must reproduce the above
  *      copyright notice, this list of conditions and the following
  *      disclaimer in the documentation and/or other materials provided
  *      with the distribution.
  *
  *    * Neither the name of Los Alamos National Security, LLC, Los
  *      Alamos National Laboratory, LANL, the U.S. Government, nor the
  *      names of its contributors may be used to endorse or promote
  *      products derived from this software without specific prior
  *      written permission.
  *
  *  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
  *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  *  DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
  *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  *  SUCH DAMAGE.
  *
  ***************************************************************************/
#include "CodeGenFunction.h"

using namespace clang;
using namespace CodeGen;

namespace{

  // this helper function can be used to extract the lambda expression
  // which may sometimes be wrapped in other AST expressions, such as
  // cast expressions
  static const LambdaExpr* GetLambda(const Expr* E){
    if(auto me = dyn_cast<MaterializeTemporaryExpr>(E)){
      E = me->GetTemporaryExpr();
    }
    
    if(const CastExpr* c = dyn_cast<CastExpr>(E)){
      E = c->getSubExpr();
    }

    if(const CXXBindTemporaryExpr* c = dyn_cast<CXXBindTemporaryExpr>(E)){
      E = c->getSubExpr();
    }

    return dyn_cast<LambdaExpr>(E);
  }

} // namespace

// invoke cleanup for Tapir

/// \brief Cleanup to ensure parent stack frame is synced.
struct RethrowCleanup : public EHScopeStack::Cleanup {
  llvm::BasicBlock *InvokeDest;
public:
  RethrowCleanup(llvm::BasicBlock *InvokeDest = nullptr)
      : InvokeDest(InvokeDest) {}
  virtual ~RethrowCleanup() {} 
  void Emit(CodeGenFunction &CGF, Flags F) {
    llvm::BasicBlock *DetRethrowBlock = CGF.createBasicBlock("det.rethrow");
    if (InvokeDest)
      CGF.Builder.CreateInvoke(
          CGF.CGM.getIntrinsic(llvm::Intrinsic::detached_rethrow),
          DetRethrowBlock, InvokeDest);
    else
      CGF.Builder.CreateBr(DetRethrowBlock);
    CGF.EmitBlock(DetRethrowBlock);
  }
};

// Helper routine copied from CodeGenFunction.cpp
static void EmitIfUsed(CodeGenFunction &CGF, llvm::BasicBlock *BB) {
  if (!BB) return;
  if (!BB->use_empty())
    return CGF.CurFn->getBasicBlockList().push_back(BB);
  delete BB;
}


static
LoopAttributes::LSStrategy GetSpawnStrategyAttribute(ArrayRef<const Attr *> Attrs) 
{
  auto A = Attrs.begin();

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
  // !!! FIXME -- this is our default setting for debugging but it    !!!
  // !!! is probably not what we want in the longrun... Probably best !!!
  // !!! to set this to DAC in a "released" version.                  !!!
  // !!!                                  -PM                         !!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  LoopAttributes::LSStrategy strategy = LoopAttributes::Sequential;
  
  while(A != Attrs.end()) {

    const attr::Kind AKind = (*A)->getKind();

    if (AKind == attr::TapirStrategy) {
      
      const auto *SA = cast<const TapirStrategyAttr>(*A);
      
      switch(SA->getTapirStrategyType()) {
      case TapirStrategyAttr::TapirSequentialStrategy:
	strategy = LoopAttributes::Sequential;
	break;
      case TapirStrategyAttr::TapirDivAndConquerStrategy:
	strategy = LoopAttributes::DAC;
	break;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
	// !!! FIXME -- Other attributes (e.g. GPU) need to be !!!
	// !!! handled here...  -PM                            !!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      default:
	strategy = LoopAttributes::Sequential;
	break;
      }
    }
    A++;
  }

  return strategy; 
}

// Perform code generation for the forall statement -- this is our our
// augmented 'for' construct that has explicit parallel semantics. In
// particular, it implies that each iteration of the loop can execute
// independently of all others.  It follows similar logic to ForStmt
// but utilizes Tapir's detach+reattach+sync constructs to express
// the parallel form.
void CodeGenFunction::EmitForallStmt(const ForallStmt &FS,
				     ArrayRef<const Attr *> ForAllAttrs) {

  if (FS.getForRangeStmt()) {
    EmitForallRangeStmt(FS, ForAllAttrs);
    return;
  }
  
  const ForStmt& S = *FS.getForStmt();
  
  JumpDest LoopExit = getJumpDestInCurrentScope("forall.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);
  
  LexicalScope ForAllScope(*this, S.getSourceRange());

  // Evaluate the first part before the loop.
  if (S.getInit())
    EmitStmt(S.getInit());

  assert(S.getCond() && "forall loop has no condition");

  // Start the loop with a block that tests the condition.
  // If there's an increment, the continue scope will be overwritten
  // later.
  JumpDest Continue = getJumpDestInCurrentScope("forall.cond");
  llvm::BasicBlock *CondBlock = Continue.getBlock();
  EmitBlock(CondBlock);
  LoopStack.setSpawnStrategy(GetSpawnStrategyAttribute(ForAllAttrs));
  
  const SourceRange &R = S.getSourceRange();
  LoopStack.push(CondBlock, CGM.getContext(), ForAllAttrs,
                 SourceLocToDebugLoc(R.getBegin()),
                 SourceLocToDebugLoc(R.getEnd()));

  const Expr *Inc = S.getInc();
  assert(Inc && "forall loop has no increment!");
  Continue = getJumpDestInCurrentScope("forall.inc");  

  JumpDest Preattach = getJumpDestInCurrentScope("forall.preattach");
  Continue = getJumpDestInCurrentScope("forall.inc");
  // Store the blocks to use for break and continue (routed through
  // reattach...    
  BreakContinueStack.push_back(BreakContinue(Preattach, Preattach));  

  // Create a cleanup scope for the condition variable cleanups.
  LexicalScope ConditionScope(*this, S.getSourceRange());

  // Save the alloca insertion point.
  llvm::AssertingVH<llvm::Instruction>  SavedAllocaInsertPt = AllocaInsertPt;

  // Save the exception handling state.
  llvm::BasicBlock *SavedEHResumeBlock  = EHResumeBlock;
  llvm::Value      *SavedExceptionSlot  = ExceptionSlot;
  llvm::AllocaInst *SavedEHSelectorSlot = EHSelectorSlot;

  llvm::BasicBlock *SyncContinueBlock = createBasicBlock("forall.end.continue");
  bool madeSync = false;
  const VarDecl *LoopVar = S.getConditionVariable();

  RValue            LoopVarInitRV;
  llvm::BasicBlock  *DetachBlock     = nullptr;
  llvm::BasicBlock  *ForAllBodyEntry = nullptr;
  llvm::BasicBlock  *ForAllBody      = nullptr;  
  {
    llvm::BasicBlock *ExitBlock = LoopExit.getBlock();

    // If there are any cleanups between here and the loop-exit scope,
    // create a block to stage a loop exit along.
    if (ForAllScope.requiresCleanups())
      ExitBlock = createBasicBlock("forall.cond.cleanup");

    DetachBlock = createBasicBlock("forall.detach");
    ForAllBodyEntry  = createBasicBlock("forall.body.entry");    
    ForAllBody       = createBasicBlock("forall.body");

    // C99 6.8.5p2/p4: The first substatement is executed if the expression
    // compares unequal to 0.  The condition must be a scalar type.
    llvm::Value *BoolCondVal = EvaluateExprAsBool(S.getCond());
    Builder.CreateCondBr(BoolCondVal, DetachBlock, ExitBlock,
			 createProfileWeightsForLoop(S.getCond(), getProfileCount(S.getBody())));

    if (ExitBlock != LoopExit.getBlock()) {
      EmitBlock(ExitBlock);
      Builder.CreateSync(SyncContinueBlock, SyncRegionStart);
      EmitBlock(SyncContinueBlock);
      PopSyncRegion();
      EmitBranchThroughCleanup(LoopExit);
      madeSync = true;      
    }

    EmitBlock(DetachBlock);
    if (LoopVar)
      LoopVarInitRV = EmitAnyExprToTemp(LoopVar->getInit());
    Builder.CreateDetach(ForAllBodyEntry, Continue.getBlock(), SyncRegionStart);

    // Create a new alloca insert point.
    llvm::Value *Undef = llvm::UndefValue::get(Int32Ty);
    AllocaInsertPt = new llvm::BitCastInst(Undef, Int32Ty, "", ForAllBodyEntry);
    // Set up a nested exception handling state.
    EHResumeBlock  = nullptr;
    ExceptionSlot  = nullptr;
    EHSelectorSlot = nullptr;

    EmitBlock(ForAllBodyEntry);
  }

  // Create a cleanup scope for the loop-variable cleanups.
  RunCleanupsScope DetachCleanupScope(*this);
  EHStack.pushCleanup<RethrowCleanup>(EHCleanup);

  // Inside the detached block, create a loop variable, setting its value
  // to the saved initialization value.
  if (LoopVar) {
    AutoVarEmission LVEmission = EmitAutoVarAlloca(*LoopVar);
    QualType type = LoopVar->getType();
    Address  Loc  = LVEmission.getObjectAddress(*this);
    LValue   LV   = MakeAddrLValue(Loc, type);
    LV.setNonGC(true);
    EmitStoreThroughLValue(LoopVarInitRV, LV, true);
    EmitAutoVarCleanups(LVEmission);
  }

  Builder.CreateBr(ForAllBody);
  EmitBlock(ForAllBody);
  incrementProfileCounter(&S);

  {
    // Create a separate cleanup scope for the body, in case it is not
    // a compound statement.
    RunCleanupsScope BodyScope(*this);
    EmitStmt(S.getBody());
    Builder.CreateBr(Preattach.getBlock());
  }

  // Finish detached body and emit the reattach.
  {
    EmitBlock(Preattach.getBlock());
    DetachCleanupScope.ForceCleanup();
    Builder.CreateReattach(Continue.getBlock(), SyncRegionStart);
  }

  // Restore CFG state after detached region.
  {
    // Restore the alloca insertion point.
    llvm::Instruction *Ptr = AllocaInsertPt;
    AllocaInsertPt = SavedAllocaInsertPt;
    Ptr->eraseFromParent();

    // Restore the exception handling state.
    EmitIfUsed(*this, EHResumeBlock);
    EHResumeBlock  = SavedEHResumeBlock;
    ExceptionSlot  = SavedExceptionSlot;
    EHSelectorSlot = SavedEHSelectorSlot;
  }

  // Emit the increment next. 
  EmitBlock(Continue.getBlock());
  EmitStmt(Inc);

  BreakContinueStack.pop_back();
  ConditionScope.ForceCleanup();

  EmitStopPoint(&S);
  EmitBranch(CondBlock);
  ForAllScope.ForceCleanup();
  LoopStack.pop();

  // Emit the fall-through block.
  EmitBlock(LoopExit.getBlock(), true);
  if (!madeSync) {
    Builder.CreateSync(SyncContinueBlock, SyncRegionStart);
    EmitBlock(SyncContinueBlock);
    PopSyncRegion();
  }

  CurFn->dump();  
}



// Perform code generation for the forall range statement -- this is a
// another version of our augmented 'forall' constructs that has explicit
// parallel semantics.  In particular, it implies that each iteration of
// the loop can execute independently of all others.  It follows a similar
// logic to ForStmt, ForAllStmt and EmitCXXForRangeStmt -- and like the
// other forall constructs, utilizes Tapir's detach+rettach+sync constructs
// to express the parallel form. 
void CodeGenFunction::EmitForallRangeStmt(const ForallStmt &FS,
					  ArrayRef<const Attr*> ForAllAttrs) {

  const CXXForRangeStmt& S = *FS.getForRangeStmt();
  
  JumpDest LoopExit = getJumpDestInCurrentScope("forall.range.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);
  
  LexicalScope ForAllScope(*this, S.getSourceRange());

  // Evaluate the first pieces before the loop.
  EmitStmt(S.getRangeStmt());
  EmitStmt(S.getBeginStmt());
  EmitStmt(S.getEndStmt());

  // Start the loop with a block that tests the condition.
  // If there's an increment, the continue scope will be overwritten
  // later.
  JumpDest Continue = getJumpDestInCurrentScope("forall.range.cond");  
  llvm::BasicBlock *CondBlock = Continue.getBlock();
  EmitBlock(CondBlock);
  LoopStack.setSpawnStrategy(GetSpawnStrategyAttribute(ForAllAttrs));
  
  const SourceRange &R = S.getSourceRange();
  LoopStack.push(CondBlock, CGM.getContext(), ForAllAttrs,
		 SourceLocToDebugLoc(R.getBegin()),
		 SourceLocToDebugLoc(R.getEnd()));

  JumpDest Preattach = getJumpDestInCurrentScope("forall.range.preattach");  
  Continue = getJumpDestInCurrentScope("forall.range.inc");

  BreakContinueStack.push_back(BreakContinue(Preattach, Preattach));

  // Create a cleanup scope for the condition variable cleanups.
  LexicalScope ConditionScope(*this, S.getSourceRange());

  // Save the alloca insertion point.
  llvm::AssertingVH<llvm::Instruction>  SavedAllocaInsertPt = AllocaInsertPt;
  
  // Save the exception handling state.
  llvm::BasicBlock *SavedEHResumeBlock  = EHResumeBlock;
  llvm::Value      *SavedExceptionSlot  = ExceptionSlot;
  llvm::AllocaInst *SavedEHSelectorSlot = EHSelectorSlot;  
  llvm::BasicBlock *SyncContinueBlock = createBasicBlock("forall.range.end.continue");
  bool madeSync = false;

  const VarDecl *LoopVar = S.getLoopVariable();  
  RValue LoopVarInitRV;
  
  llvm::BasicBlock *ForAllBody      = createBasicBlock("forall.range.body");
  llvm::BasicBlock *DetachBlock     = createBasicBlock("forall.range.detach");
  llvm::BasicBlock *ForAllBodyEntry = createBasicBlock("forall.range.body.entry");
  {

    // If there are any cleanups between here and the loop-exit scope,
    // create a block to state a loop exit...
    llvm::BasicBlock *ExitBlock = LoopExit.getBlock();  
    if (ForAllScope.requiresCleanups())
      ExitBlock = createBasicBlock("forall.cond.cleanup");
  
  
    // The body is executed if the expression, contextually converted
    // to bool, is true.
    llvm::Value *BoolCondVal = EvaluateExprAsBool(S.getCond());
    Builder.CreateCondBr(
			 BoolCondVal, DetachBlock, ExitBlock,
			 createProfileWeightsForLoop(S.getCond(), getProfileCount(S.getBody())));  
  
  
    if (ExitBlock != LoopExit.getBlock()) {
      EmitBlock(ExitBlock);
      Builder.CreateSync(SyncContinueBlock, SyncRegionStart);
      EmitBlock(SyncContinueBlock);
      PopSyncRegion();
      EmitBranchThroughCleanup(LoopExit);
      madeSync = true;
    }

    EmitBlock(DetachBlock);

    if (LoopVar) 
      LoopVarInitRV = EmitAnyExprToTemp(LoopVar->getInit());
    
    Builder.CreateDetach(ForAllBodyEntry, Continue.getBlock(), SyncRegionStart);
  
    // Create a new alloca insert point...
    llvm::Value *Undef = llvm::UndefValue::get(Int32Ty);
    AllocaInsertPt = new llvm::BitCastInst(Undef, Int32Ty, "", ForAllBodyEntry);  

    // Set up a nested exception handling state.
    EHResumeBlock  = nullptr;
    ExceptionSlot  = nullptr;
    EHSelectorSlot = nullptr;
  
    EmitBlock(ForAllBodyEntry);
  }
  // Create a cleanup scope for the loop-variable cleanups.
  RunCleanupsScope DetachCleanupScope(*this);
  EHStack.pushCleanup<RethrowCleanup>(EHCleanup);

  /*
  if (LoopVar) {
  AutoVarEmission LVEmission = EmitAutoVarAlloca(*LoopVar);
  QualType type = LoopVar->getType();
  Address Loc = LVEmission.getObjectAddress(*this);
  LValue LV = MakeAddrLValue(Loc, type);
  LV.setNonGC(true);
  EmitStoreThroughLValue(LoopVarInitRV, LV, true);
  EmitAutoVarCleanups(LVEmission);
  */
  }
  Builder.CreateBr(ForAllBody);
  EmitBlock(ForAllBody);
  incrementProfileCounter(&S);
  
  // Create a separate cleanup scope for the loop variable and body.
  {
    //LexicalScope BodyScope(*this, S.getSourceRange());
    RunCleanupsScope BodyScope(*this);
    EmitStmt(S.getLoopVarStmt());
    EmitStmt(S.getBody());
  }
  // Not sure why this has to move out of the scope above... 
  Builder.CreateBr(Preattach.getBlock());          
  
  {
    EmitBlock(Preattach.getBlock());
    DetachCleanupScope.ForceCleanup();
    Builder.CreateReattach(Continue.getBlock(), SyncRegionStart);
  }

  // Restore CFG state after insertion point.
  {
    llvm::Instruction *Ptr = AllocaInsertPt;
    AllocaInsertPt = SavedAllocaInsertPt;
    Ptr->eraseFromParent();
    // Restore the exception handling state.
    EmitIfUsed(*this, EHResumeBlock);
    EHResumeBlock  = SavedEHResumeBlock;
    ExceptionSlot  = SavedExceptionSlot;
    EHSelectorSlot = SavedEHSelectorSlot;
  }
  
  // If there is an increment, emit it next.
  EmitBlock(Continue.getBlock());
  EmitStmt(S.getInc());  
  BreakContinueStack.pop_back();
  ConditionScope.ForceCleanup();
  
  EmitStopPoint(&S);
  EmitBranch(CondBlock);
  ForAllScope.ForceCleanup();
  LoopStack.pop();

  // Emit the fall-through block.
  EmitBlock(LoopExit.getBlock(), true);
  if (! madeSync) {
    Builder.CreateSync(SyncContinueBlock, SyncRegionStart);
    EmitBlock(SyncContinueBlock);
    PopSyncRegion();
  }
}


// this method handles the code generation corresponding to Kokkos parallel for
// or parallel reduce
void CodeGenFunction::EmitKokkosConstruct(const CallExpr* E){
  // anything to include here?
  ArrayRef<const Attr *> ForAttrs;

  // extract the lambda expression which will be used as a body of the
  // parallel for loop
  const LambdaExpr* LE = GetLambda(E->getArg(1));
  const CXXMethodDecl* MD = LE->getCallOperator();
  const ParmVarDecl* LoopVar = MD->getParamDecl(0);

  const FunctionDecl* F = E->getDirectCallee();
  assert(F);

  const VarDecl* ReduceVar;

  if(F->getQualifiedNameAsString() == "Kokkos::parallel_for"){
    assert(MD->getNumParams() == 1);
    assert(E->getNumArgs() == 3);
    ReduceVar = nullptr;
  }
  else if(F->getQualifiedNameAsString() == "Kokkos::parallel_reduce"){
    ReduceVar = MD->getParamDecl(1);
    assert(MD->getNumParams() == 2);
  }
  else{
    assert(false && "expected parallel for or reduce");
  }

  JumpDest LoopExit = getJumpDestInCurrentScope("pfor.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);

  LexicalScope ForScope(*this, E->getSourceRange());

  // emit the loop variable whose declaration comes as the sole argument
  // to the lambda expression, and initialize it to zero
  EmitVarDecl(*LoopVar);
  Address Addr = GetAddrOfLocalVar(LoopVar);
  llvm::Value *Zero = llvm::ConstantInt::get(ConvertType(LoopVar->getType()), 0);
  Builder.CreateStore(Zero, Addr);

  if(ReduceVar){
    EmitVarDecl(*ReduceVar);
  }

  // calculate the end range

  llvm::Value *N = EmitScalarExpr(E->getArg(0));

  llvm::Type *LoopVarTy = ConvertType(LoopVar->getType());

  unsigned NBits = N->getType()->getPrimitiveSizeInBits();
  unsigned LoopVarBits = LoopVarTy->getPrimitiveSizeInBits();

  // we may need to truncate or extend the end range to get it to match the
  // type of the loop variable
  if(NBits > LoopVarBits){
    N = Builder.CreateTrunc(N, LoopVarTy);
  }
  else if(NBits < LoopVarBits){
    N = Builder.CreateZExt(N, LoopVarTy);
  }

  JumpDest Continue = getJumpDestInCurrentScope("pfor.cond");
  llvm::BasicBlock *CondBlock = Continue.getBlock();
  EmitBlock(CondBlock);

  LoopStack.setSpawnStrategy(LoopAttributes::DAC);
  const SourceRange &R = E->getSourceRange();

  LoopStack.push(CondBlock, CGM.getContext(), ForAttrs,
                 SourceLocToDebugLoc(R.getBegin()),
                 SourceLocToDebugLoc(R.getEnd()));

  JumpDest Preattach = getJumpDestInCurrentScope("pfor.preattach");
  Continue = getJumpDestInCurrentScope("pfor.inc");

  // Store the blocks to use for break and continue.
  BreakContinueStack.push_back(BreakContinue(Preattach, Preattach));

  // Create a cleanup scope for the condition variable cleanups.
  LexicalScope ConditionScope(*this, R);

  // Save the old alloca insert point.
  llvm::AssertingVH<llvm::Instruction> OldAllocaInsertPt = AllocaInsertPt;
  // Save the old EH state.
  llvm::BasicBlock *OldEHResumeBlock = EHResumeBlock;
  llvm::Value *OldExceptionSlot = ExceptionSlot;
  llvm::AllocaInst *OldEHSelectorSlot = EHSelectorSlot;

  llvm::BasicBlock *SyncContinueBlock = createBasicBlock("pfor.end.continue");
  bool madeSync = false;
  llvm::BasicBlock *DetachBlock;
  llvm::BasicBlock *ForBodyEntry;
  llvm::BasicBlock *ForBody;

  {
    llvm::BasicBlock *ExitBlock = LoopExit.getBlock();
    // If there are any cleanups between here and the loop-exit scope,
    // create a block to stage a loop exit along.
    if (ForScope.requiresCleanups())
      ExitBlock = createBasicBlock("pfor.cond.cleanup");

    // As long as the condition is true, iterate the loop.
    DetachBlock = createBasicBlock("pfor.detach");
    // Emit extra entry block for detached body, to ensure that this detached
    // entry block has just one predecessor.
    ForBodyEntry = createBasicBlock("pfor.body.entry");
    ForBody = createBasicBlock("pfor.body");

    llvm::Value *LoopVal = Builder.CreateLoad(Addr);

    llvm::Value *BoolCondVal = Builder.CreateICmpULT(LoopVal, N);

    // create profile from N?
/*
    Builder.CreateCondBr(
        BoolCondVal, DetachBlock, ExitBlock,
        createProfileWeightsForLoop(S.getCond(), getProfileCount(S.getBody())));
*/

    Builder.CreateCondBr(
        BoolCondVal, DetachBlock, ExitBlock);

    if (ExitBlock != LoopExit.getBlock()) {
      EmitBlock(ExitBlock);
      Builder.CreateSync(SyncContinueBlock, SyncRegionStart);
      EmitBlock(SyncContinueBlock);
      PopSyncRegion();
      madeSync = true;
      EmitBranchThroughCleanup(LoopExit);
    }
  
    EmitBlock(DetachBlock);

    Builder.CreateDetach(ForBodyEntry, Continue.getBlock(), SyncRegionStart);

    // Create a new alloca insert point.
    llvm::Value *Undef = llvm::UndefValue::get(Int32Ty);
    AllocaInsertPt = new llvm::BitCastInst(Undef, Int32Ty, "", ForBodyEntry);
    // Set up nested EH state.
    EHResumeBlock = nullptr;
    ExceptionSlot = nullptr;
    EHSelectorSlot = nullptr;

    EmitBlock(ForBodyEntry);  
  }

  // Create a cleanup scope for the loop-variable cleanups.
  RunCleanupsScope DetachCleanupsScope(*this);
  EHStack.pushCleanup<RethrowCleanup>(EHCleanup);

  Builder.CreateBr(ForBody);

  EmitBlock(ForBody);

  incrementProfileCounter(E);

  {
    // Create a separate cleanup scope for the body, in case it is not
    // a compound statement.
    RunCleanupsScope BodyScope(*this);
    
    // emit the lambda expression as the body forall a loop
    // since it came from a lambda, it may have special wrapped AST's
    // for handling captured variables, so we set the following flag to true
    // so that we can handle these has a special case, and look them up
    // directly in the local declaration map instead
    InKokkosConstruct = true;
    EmitStmt(LE->getBody());
    InKokkosConstruct = false;
    Builder.CreateBr(Preattach.getBlock());
  }

  // Finish detached body and emit the reattach.
  {
    EmitBlock(Preattach.getBlock());

    DetachCleanupsScope.ForceCleanup();

    Builder.CreateReattach(Continue.getBlock(), SyncRegionStart);
  }

  // Restore CGF state after detached region.
  {
    // Restore the alloca insertion point.
    llvm::Instruction *Ptr = AllocaInsertPt;
    AllocaInsertPt = OldAllocaInsertPt;
    Ptr->eraseFromParent();

    // Restore the EH state.
    EmitIfUsed(*this, EHResumeBlock);
    EHResumeBlock = OldEHResumeBlock;
    ExceptionSlot = OldExceptionSlot;
    EHSelectorSlot = OldEHSelectorSlot;
  }

  // Emit the increment next.
  EmitBlock(Continue.getBlock());

  // emit the loop variable increment
  llvm::Value *IncVal = Builder.CreateLoad(Addr);
  llvm::Value *One = llvm::ConstantInt::get(ConvertType(LoopVar->getType()), 1);
  IncVal = Builder.CreateAdd(IncVal, One);
  Builder.CreateStore(IncVal, Addr);

  BreakContinueStack.pop_back();

  ConditionScope.ForceCleanup();

  EmitStopPoint(E);
  EmitBranch(CondBlock);

  ForScope.ForceCleanup();

  LoopStack.pop();
  // Emit the fall-through block.
  EmitBlock(LoopExit.getBlock(), true);
  if (!madeSync) {
    Builder.CreateSync(SyncContinueBlock, SyncRegionStart);
    EmitBlock(SyncContinueBlock);
    PopSyncRegion();
  }

  // debug statement
  //CurFn->dump();
}
