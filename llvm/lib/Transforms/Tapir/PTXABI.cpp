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

#define np(X)                                                            \
 std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ \
           << ": " << #X << " = " << (X) << std::endl

#include "llvm/Transforms/Tapir/PTXABI.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Tapir/Outline.h"
#include "llvm/Transforms/Utils/EscapeEnumerator.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/TapirUtils.h"

#include <iostream>
#include <set>
#include <sstream>

// +===== Kitsune

#define np(X)                                                            \
 std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ \
           << ": " << #X << " = " << (X) << std::endl

#include <iostream>
#include <set>
#include <sstream>

// ==============

using namespace llvm;

//##############################################################################

PTXABI::PTXABI() {}

/// \brief Get/Create the worker count for the spawning function.
Value *PTXABI::GetOrCreateWorker8(Function &F) {
  Module *M = F.getParent(); 
  LLVMContext& C = M->getContext();
  return ConstantInt::get(C, APInt(16, 8));
}

void PTXABI::createSync(SyncInst &SI, ValueToValueMapTy &DetachCtxToStackFrame) {
}

Function *PTXABI::createDetach(DetachInst &detach,
                               ValueToValueMapTy &DetachCtxToStackFrame,
                               DominatorTree &DT, AssumptionCache &AC) {
  BasicBlock *detB = detach.getParent();
  // unused -- Function &F = *(detB->getParent());

  BasicBlock *Spawned  = detach.getDetached();
  BasicBlock *Continue = detach.getContinue();

  // unused -- Module *M = F.getParent();

  CallInst *cal = nullptr;
  Function *extracted = extractDetachBodyToFunction(detach, DT, AC, &cal);
  //extracted = formatFunctionToTask(extracted, cal);

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
  return extracted;
}

void PTXABI::preProcessFunction(Function &F) {
}

void PTXABI::postProcessFunction(Function &F) {
}

void PTXABI::postProcessHelper(Function &F) {
}

bool PTXABI::processMain(Function &F) {
  return true;
}

bool PTXABILoopSpawning::processLoop(){
  Loop *L = OrigLoop;

  //  code generation is currently limited to a simple canonical loop structure
  //  whereby we make the following assumptions and check assertions below
  //  soon we will expand this extraction mechanism to handle more complex
  //  loops

  using TypeVec = std::vector<Type*>;

  LLVMContext& c = L->getHeader()->getContext();

  IRBuilder<> b(c);

  Type* voidTy = Type::getVoidTy(c);

  //  and LLVM transformation is able in some cases to transform the loop to 
  //  contain a phi node that exists at the entry block

  PHINode* loopNode = L->getCanonicalInductionVariable();
  assert(loopNode && "expected canonical loop");

  //  only handle loops where the induction variable is initialized to a constant

  ConstantInt* loopStart = dyn_cast<ConstantInt>(loopNode->getIncomingValue(0));
  assert(loopStart && "expected canonical loop start");

  BasicBlock* exitBlock = L->getUniqueExitBlock();
  assert(exitBlock && "expected canonical exit block");

  // and assume that a branch instruction exists here

  BasicBlock* branchBlock = exitBlock->getSinglePredecessor();
  assert(branchBlock && "expected canonical branch block");

  BranchInst* endBranch = dyn_cast<BranchInst>(branchBlock->getTerminator());
  assert(endBranch && "expected canonical end branch instruction");

  //  get the branch condition in order to extract the end loop value
  //  which we also currently assume is constant

  Value* endBranchCond = endBranch->getCondition();
  CmpInst* cmp = dyn_cast<CmpInst>(endBranchCond);
  assert(cmp && "expected canonical comparison instruction");

  ConstantInt* loopEnd = dyn_cast<ConstantInt>(cmp->getOperand(1));
  assert(loopEnd && "expected canonical loop end");

  BasicBlock* entryBlock = L->getBlocks()[0];

  // assume a detach exists here  and this basic block contains the body
  //  of the kernel function we will be generating

  DetachInst* detach = dyn_cast<DetachInst>(entryBlock->getTerminator());
  assert(detach && "expected canonical loop entry detach");

  BasicBlock* Body = detach->getDetached();

  // extract the externally defined variables
  // these will be passed in as CUDA arrays

  std::set<Value*> values;
  values.insert(loopNode);

  std::set<Value*> extValues;

  for(Instruction& ii : *Body){
    if(dyn_cast<ReattachInst>(&ii)){
      continue;
    }

    for(Use& u : ii.operands()){
      Value* v = u.get();

      if(values.find(v) == values.end()){
        extValues.insert(v);
      }
    }
    
    values.insert(&ii);
  }

  TypeVec paramTypes;

  for(Value* v : extValues){
    paramTypes.push_back(v->getType());
  }

  // create the GPU function

  FunctionType* funcTy = FunctionType::get(voidTy, paramTypes, false);

  Module PTXModule("PTXModule", c);

  llvm::Function* f = llvm::Function::Create(funcTy,
    llvm::Function::ExternalLinkage, "run", &PTXModule);

  auto aitr = f->arg_begin();

  std::map<Value*, Value*> m;

  // set and parameter names and map values to be replaced

  size_t i = 0;

  for(Value* v : extValues){
    std::stringstream sstr;
    sstr << "arg" << i;

    m[v] = aitr;
    aitr->setName(sstr.str());
    ++aitr;
    ++i;
  }

  BasicBlock* br = BasicBlock::Create(c, "entry", f);
  BasicBlock::InstListType& il = br->getInstList();

  // clone instructions of the body basic block,  remapping values as needed

  for(Instruction& ii : *Body){
    if(dyn_cast<ReattachInst>(&ii)){
      continue;
    }

    Instruction* ic = ii.clone();

    for(auto& itr : m){
      ic->replaceUsesOfWith(itr.first, itr.second);
    }

    il.push_back(ic);
    m[&ii] = ic;
  }

  b.SetInsertPoint(br);

  b.CreateRetVoid();

  //PTXModule.dump();

  // L->dump();
  // for(auto B : L->blocks()){
  //   B->dump();
  // }

  return true;
}
