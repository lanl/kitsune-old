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

#include "CGKitsuneRuntime.h"
#include "CodeGenFunction.h"

using namespace std;
using namespace clang;
using namespace CodeGen;
using namespace llvm;

CGKitsuneRuntime::CGKitsuneRuntime(llvm::Module& module)
: module_(module){
  llvm::LLVMContext& C = module_.getContext();
 
  Int1Ty = llvm::IntegerType::getInt1Ty(C);
  Int8Ty = llvm::IntegerType::getInt8Ty(C);
  Int32Ty = llvm::IntegerType::getInt32Ty(C);
  Int64Ty = llvm::IntegerType::getInt64Ty(C);
  FloatTy = llvm::Type::getFloatTy(C);
  DoubleTy = llvm::Type::getDoubleTy(C);
  VoidTy = llvm::Type::getVoidTy(C);
  VoidPtrTy = PointerTy(Int8Ty);
  StringTy = PointerTy(Int8Ty);
  
  TypeVec params = {VoidPtrTy};
}

CGKitsuneRuntime::CGKitsuneRuntime(CodeGenModule& CGM)
: module_(CGM.getModule()){
  llvm::LLVMContext& C = module_.getContext();
 
  Int1Ty = llvm::IntegerType::getInt1Ty(C);
  Int8Ty = llvm::IntegerType::getInt8Ty(C);
  Int32Ty = llvm::IntegerType::getInt32Ty(C);
  Int64Ty = llvm::IntegerType::getInt64Ty(C);
  FloatTy = llvm::Type::getFloatTy(C);
  DoubleTy = llvm::Type::getDoubleTy(C);
  VoidTy = llvm::Type::getVoidTy(C);
  VoidPtrTy = PointerTy(Int8Ty);
  StringTy = PointerTy(Int8Ty);
  
  TypeVec params = {VoidPtrTy};
}

CGKitsuneRuntime::~CGKitsuneRuntime(){}

Value* CGKitsuneRuntime::GetNull(llvm::Type* T){
  return ConstantPointerNull::get(PointerTy(T));
}

llvm::PointerType* CGKitsuneRuntime::PointerTy(llvm::Type* elementType){
  return llvm::PointerType::get(elementType, 0);
}

llvm::Function*
CGKitsuneRuntime::GetFunc(const std::string& funcName,
                        const TypeVec& argTypes,
                        llvm::Type* retType){

  llvm::LLVMContext& C = module_.getContext();
  
  llvm::Function* func = module_.getFunction(funcName);
  
  if(!func){
    llvm::FunctionType* funcType =
    llvm::FunctionType::get(retType == 0 ?
                            llvm::Type::getVoidTy(C) : retType,
                            argTypes, false);
    
    func =
    llvm::Function::Create(funcType,
                           llvm::Function::ExternalLinkage,
                           funcName,
                           &module_);
  }
  
  return func;
}

llvm::Function*
CGKitsuneRuntime::getSREGFunc(const string& suffix){
  return GetFunc("llvm.nvvm.read.ptx.sreg." + suffix, TypeVec(), Int32Ty);
}

llvm::Function*
CGKitsuneRuntime::getBarrierFunc(){
  return GetFunc("llvm.nvvm.barrier0", TypeVec());
}

llvm::Function*
CGKitsuneRuntime::CudaLaunchDeviceFunc(){
  llvm::Type* pt = llvm::ArrayType::get(Int8Ty, 12);
  //pt->setAlignment(4);

  return GetFunc("cudaLaunchDevice",
    {VoidPtrTy, VoidPtrTy, pt, pt, Int32Ty, VoidPtrTy}, Int32Ty);
}
