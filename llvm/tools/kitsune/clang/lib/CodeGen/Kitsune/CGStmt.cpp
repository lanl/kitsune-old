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

void CodeGenFunction::EmitForallStmt(const ForallStmt &S,
                                     ArrayRef<const Attr *> ForAttrs) {
  JumpDest LoopExit = getJumpDestInCurrentScope("pfor.end");

  PushSyncRegion();
  llvm::Instruction *SyncRegionStart = EmitSyncRegionStart();
  CurSyncRegion->setSyncRegionStart(SyncRegionStart);

  LexicalScope ForScope(*this, S.getSourceRange());

  const VarDecl* IndVar = S.getIndVar();

/*
  EmitVarDecl(*IndVar);

  Address IndAddr = GetAddrOfLocalVar(IndVar);

  llvm::Value* IndVal = Builder.CreateLoad(IndAddr, "ind");
  */

  RValue Size = EmitAnyExprToTemp(S.getSize());

  llvm::Value* SizeVal = Size.getScalarVal();

  JumpDest Continue = getJumpDestInCurrentScope("pfor.cond");
  llvm::BasicBlock *CondBlock = Continue.getBlock();
  EmitBlock(CondBlock);

  LoopStack.setSpawnStrategy(LoopAttributes::DAC);
  const SourceRange &R = S.getSourceRange();
  LoopStack.push(CondBlock, CGM.getContext(), ForAttrs,
                 SourceLocToDebugLoc(R.getBegin()),
                 SourceLocToDebugLoc(R.getEnd()));

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

  // Inside the detached block, create the loop variable, setting its value to
  // the saved initialization value.
  AutoVarEmission LVEmission = EmitAutoVarAlloca(*IndVar);
  QualType type = IndVar->getType();
  Address Loc = LVEmission.getObjectAddress(*this);
  llvm::Value* IndVal = Builder.CreateLoad(Loc, "ind.val");
  LValue LV = MakeAddrLValue(Loc, type);
  LV.setNonGC(true);
  EmitStoreThroughLValue(RValue::get(llvm::ConstantInt::get(Int32Ty, 0)), LV, true);
  EmitAutoVarCleanups(LVEmission);

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

    llvm::Value *BoolCondVal = 
      Builder.CreateICmpUGT(IndVal, SizeVal, "end.cond");

    Builder.CreateCondBr(BoolCondVal, DetachBlock, ExitBlock);

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

  IndVal = Builder.CreateLoad(Loc, "ind.val");
  
  llvm::Value* Inc = 
    Builder.CreateAdd(IndVal, llvm::ConstantInt::get(Int32Ty, 1));

  EmitStoreThroughLValue(RValue::get(Inc), LV, true);

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
}
