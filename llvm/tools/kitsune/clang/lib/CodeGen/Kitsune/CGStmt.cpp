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

llvm::Instruction *CodeGenFunction::EmitSyncRegionStart() {
  // Start the sync region.  To ensure the syncregion.start call dominates all
  // uses of the generated token, we insert this call at the alloca insertion
  // point.
  llvm::Instruction *SRStart = llvm::CallInst::Create(
      CGM.getIntrinsic(llvm::Intrinsic::syncregion_start),
      "syncreg", AllocaInsertPt);
  return SRStart;
}

/// \brief Cleanup to ensure parent stack frame is synced.
struct RethrowCleanup : public EHScopeStack::Cleanup {
  llvm::BasicBlock *InvokeDest;
public:
  RethrowCleanup(llvm::BasicBlock *InvokeDest = nullptr)
      : InvokeDest(InvokeDest) {}
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

void CodeGenFunction::EmitForallStmt(const ForallStmt &FS,
                                     ArrayRef<const Attr *> ForAttrs) {
  if(FS.getForRangeStmt()){
    EmitForallRangeStmt(FS, ForAttrs);
    return;
  }

  const ForStmt& S = *FS.getForStmt();

  JumpDest LoopExit = getJumpDestInCurrentScope("pfor.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);

  LexicalScope ForScope(*this, S.getSourceRange());

  EmitStmt(S.getInit());

  JumpDest Continue = getJumpDestInCurrentScope("pfor.cond");
  llvm::BasicBlock *CondBlock = Continue.getBlock();
  EmitBlock(CondBlock);

  LoopStack.setSpawnStrategy(LoopAttributes::DAC);
  const SourceRange &R = S.getSourceRange();
  LoopStack.push(CondBlock, CGM.getContext(), ForAttrs,
                 SourceLocToDebugLoc(R.getBegin()),
                 SourceLocToDebugLoc(R.getEnd()));

  const Expr *Inc = S.getInc();

  JumpDest Preattach = getJumpDestInCurrentScope("pfor.preattach");
  Continue = getJumpDestInCurrentScope("pfor.inc");

  // Store the blocks to use for break and continue.
  BreakContinueStack.push_back(BreakContinue(Preattach, Preattach));

  // Create a cleanup scope for the condition variable cleanups.
  LexicalScope ConditionScope(*this, S.getSourceRange());

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

    llvm::Value *BoolCondVal = EvaluateExprAsBool(S.getCond());

    Builder.CreateCondBr(
        BoolCondVal, DetachBlock, ExitBlock,
        createProfileWeightsForLoop(S.getCond(), getProfileCount(S.getBody())));

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
  EmitStmt(Inc);

  BreakContinueStack.pop_back();

  ConditionScope.ForceCleanup();

  EmitStopPoint(&S);
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

  //CurFn->dump();
}

void CodeGenFunction::EmitForallRangeStmt(const ForallStmt &FS,
                                          ArrayRef<const Attr *> ForAttrs) {
  const CXXForRangeStmt& S = *FS.getForRangeStmt();

  JumpDest LoopExit = getJumpDestInCurrentScope("pfor.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);

  LexicalScope ForScope(*this, S.getSourceRange());

  // Evaluate the first pieces before the loop.
  EmitStmt(S.getRangeStmt());
  EmitStmt(S.getBeginStmt());
  EmitStmt(S.getEndStmt());

  // Start the loop with a block that tests the condition.
  // If there's an increment, the continue scope will be overwritten
  // later.
  llvm::BasicBlock *CondBlock = createBasicBlock("pfor.cond");
  EmitBlock(CondBlock);

  const SourceRange &R = S.getSourceRange();
  LoopStack.push(CondBlock, CGM.getContext(), ForAttrs,
                 SourceLocToDebugLoc(R.getBegin()),
                 SourceLocToDebugLoc(R.getEnd()));

  // If there are any cleanups between here and the loop-exit scope,
  // create a block to stage a loop exit along.
  llvm::BasicBlock *ExitBlock = LoopExit.getBlock();
  if (ForScope.requiresCleanups())
    ExitBlock = createBasicBlock("pfor.cond.cleanup");

  llvm::BasicBlock *SyncContinueBlock = createBasicBlock("pfor.end.continue");
  bool madeSync = false;
  llvm::BasicBlock *DetachBlock = createBasicBlock("pfor.detach");

  // The loop body, consisting of the specified body and the loop variable.
  llvm::BasicBlock *ForBody = createBasicBlock("pfor.body");

  // The body is executed if the expression, contextually converted
  // to bool, is true.
  llvm::Value *BoolCondVal = EvaluateExprAsBool(S.getCond());
  Builder.CreateCondBr(
      BoolCondVal, DetachBlock, ExitBlock,
      createProfileWeightsForLoop(S.getCond(), getProfileCount(S.getBody())));

  if (ExitBlock != LoopExit.getBlock()) {
    EmitBlock(ExitBlock);
    EmitBranchThroughCleanup(LoopExit);
  }

  // right place to call this?
  JumpDest Continue = getJumpDestInCurrentScope("pfor.inc");

  EmitBlock(DetachBlock);

  Builder.CreateDetach(ForBody, Continue.getBlock(), SyncRegionStart);

  EmitBlock(ForBody);

  RunCleanupsScope DetachCleanupsScope(*this);

  incrementProfileCounter(&S);

  // Create a block for the increment. In case of a 'continue', we jump there.
  JumpDest Preattach = getJumpDestInCurrentScope("pfor.preattach");

  // Store the blocks to use for break and continue.
  BreakContinueStack.push_back(BreakContinue(LoopExit, Continue));

  {
    // Create a separate cleanup scope for the loop variable and body.
    LexicalScope BodyScope(*this, S.getSourceRange());
    EmitStmt(S.getLoopVarStmt());
    EmitStmt(S.getBody());
    Builder.CreateBr(Preattach.getBlock());
  }

  EmitBlock(Preattach.getBlock());
  DetachCleanupsScope.ForceCleanup();
  Builder.CreateReattach(Continue.getBlock(), SyncRegionStart);

  EmitStopPoint(&S);
  // If there is an increment, emit it next.
  EmitBlock(Continue.getBlock());
  EmitStmt(S.getInc());

  BreakContinueStack.pop_back();

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

  //CurFn->dump();
}

void CodeGenFunction::EmitKokkosConstruct(const CallExpr* E){
  // anything to include here?
  ArrayRef<const Attr *> ForAttrs;

  const LambdaExpr* LE = GetLambda(E->getArg(1));
  const CXXMethodDecl* MD = LE->getCallOperator();
  const ParmVarDecl* LoopVar = MD->getParamDecl(0);

  JumpDest LoopExit = getJumpDestInCurrentScope("pfor.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);

  LexicalScope ForScope(*this, E->getSourceRange());

  EmitVarDecl(*LoopVar);
  Address Addr = GetAddrOfLocalVar(LoopVar);
  llvm::Value *Zero = llvm::ConstantInt::get(ConvertType(LoopVar->getType()), 0);
  Builder.CreateStore(Zero, Addr);

  llvm::Value *N = EmitScalarExpr(E->getArg(0));

  llvm::Type *LoopVarTy = ConvertType(LoopVar->getType());

  unsigned NBits = N->getType()->getPrimitiveSizeInBits();
  unsigned LoopVarBits = LoopVarTy->getPrimitiveSizeInBits();

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
    EmitStmt(LE->getBody());
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

  //CurFn->dump();
}
