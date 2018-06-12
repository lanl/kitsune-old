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


#include "kitsune/GPURuntime.h"

#include <iostream>

#include <map>
#include <vector>
#include <cassert>
#include <cmath>

#include <cuda.h>

using namespace std;
using namespace kitsune;

static const uint8_t FIELD_READ = 0x01;
static const uint8_t FIELD_WRITE = 0x02;

static const size_t DEFAULT_THREADS = 128;

class CUDARuntime : public GPURuntime{
public:
  static void check(CUresult err){
    if(err != CUDA_SUCCESS){
      const char* s;
      cuGetErrorString(err, &s);
      cerr << "CUDARuntime error: " << s << endl;
      assert(false);
    }
  }

  class CommonField{
  public:
    CommonField(void* hostPtr, CUdeviceptr devPtr, size_t size)
      : hostPtr(hostPtr),
        devPtr(devPtr),
        size(size){

    }

    ~CommonField(){
      CUresult err = cuMemFree(devPtr);
      check(err);
    }

    void* hostPtr;
    CUdeviceptr devPtr;
    size_t size;
  };

  class CommonData{
  public:
    CommonData(){}

    ~CommonData(){
      for(auto& itr : fieldMap_){
        delete itr.second;
      }
    }

    CommonField* getField(const char* name){
      auto itr = fieldMap_.find(name);
      if(itr != fieldMap_.end()){
        return itr->second;
      }

      return 0;
    }

    CommonField* addField(const char* name,
                          void* hostPtr,
                          uint32_t elementSize,
                          uint64_t size){

      size *= elementSize;

      CUdeviceptr devPtr;
      CUresult err = cuMemAlloc(&devPtr, size);
      check(err);

      CommonField* field = new CommonField(hostPtr, devPtr, size);
      fieldMap_[name] = field;
      return field;
    }

  private:
    typedef map<const char*, CommonField*> FieldMap_;

    FieldMap_ fieldMap_;
  };

  class Kernel;

  class PTXModule{
  public:    
    PTXModule(const char* ptx){
      CUresult err = cuModuleLoadData(&module_, (void*)ptx);
      check(err);
    }

    Kernel* createKernel(CommonData* commonData, const char* kernelName);

  private:
    CUmodule module_;
  };

  class Kernel{
  public:
    class Field{
    public:
      CommonField* commonField;
      uint8_t mode;

      bool isRead(){
        return mode & FIELD_READ;
      }

      bool isWrite(){
        return mode & FIELD_WRITE;
      }
    };

    Kernel(PTXModule* module,
           CommonData* commonData,
           CUfunction function)
      : module_(module),
        commonData_(commonData),
        function_(function),
        ready_(false),
        numThreads_(DEFAULT_THREADS){
      
    }

    ~Kernel(){
      for(auto& itr : fieldMap_){
        delete itr.second;
      }
    }
    
    void setNumThreads(size_t numThreads){
      numThreads_ = numThreads;
    }

    void addField(const char* fieldName,
                  CommonField* commonField,
                  uint8_t mode){

      Field* field = new Field;
      field->commonField = commonField;
      field->mode = mode;

      fieldMap_.insert({fieldName, field});
    }

    void run(){
      if(!ready_){
        for(auto& itr : fieldMap_){
          Field* field = itr.second;
          CommonField* commonField = field->commonField;
          kernelParams_.push_back(&commonField->devPtr);
        }
        ready_ = true;
      }

      CUresult err;

      for(auto& itr : fieldMap_){
        Field* field = itr.second;
        if(field->isRead()){
          CommonField* commonField = field->commonField;
          err = cuMemcpyHtoD(commonField->devPtr, commonField->hostPtr,
                             commonField->size);
          check(err);
        }
      }

      err = cuLaunchKernel(function_, 1, 1, 1,
                           numThreads_, 1, 1, 
                           0, NULL, kernelParams_.data(), NULL);
      check(err);

      for(auto& itr : fieldMap_){
        Field* field = itr.second;
        if(field->isWrite()){
          CommonField* commonField = field->commonField;
          err = cuMemcpyDtoH(commonField->hostPtr, commonField->devPtr, 
                             commonField->size);
          check(err);
        }
      }
    }
    
    PTXModule* module(){
      return module_;
    }

    CommonData* commonData(){
      return commonData_;
    }

    bool ready(){
      return ready_;
    }
    
  private:    
    typedef map<string, Field*> FieldMap_;
    typedef vector<void*> KernelParams_;

    CUfunction function_;
    PTXModule* module_;
    CommonData* commonData_;
    bool ready_;
    FieldMap_ fieldMap_;
    KernelParams_ kernelParams_;
    size_t numThreads_;
  };

  CUDARuntime(){

  }

  ~CUDARuntime(){
    CUresult err = cuCtxDestroy(context_);
    check(err);

    for(auto& itr : kernelMap_){
      delete itr.second;
    }

    for(auto& itr : moduleMap_){
      delete itr.second;
    }
  }

  void init(){
    CUresult err = cuInit(0);
    check(err);
    
    err = cuDeviceGet(&device_, 0);
    check(err);

    err = cuCtxCreate(&context_, 0, device_);
    check(err);

    int threadsPerBlock;
    err = 
      cuDeviceGetAttribute(&threadsPerBlock,
                           CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, device_);
    check(err);
    numThreads_ = threadsPerBlock;
  }

  void initKernel(const char* ptx,
                  const char* kernelName){

    auto kitr = kernelMap_.find(kernelName);
    if(kitr != kernelMap_.end()){
      return;
    }

    PTXModule* module;
    auto mitr = moduleMap_.find(ptx);
    if(mitr != moduleMap_.end()){
      module = mitr->second;
    }
    else{
      module = new PTXModule(ptx);
      moduleMap_[kernelName] = module;
    }

    Kernel* kernel = module->createKernel(nullptr, kernelName);
    kernel->setNumThreads(numThreads_);
    kernelMap_.insert({kernelName, kernel});
  }

  void initField(const char* kernelName,
                 const char* fieldName,
                 void* hostPtr,
                 uint32_t elementSize,
                 uint64_t size,
                 uint8_t mode){

    auto kitr = kernelMap_.find(kernelName);
    assert(kitr != kernelMap_.end() && "invalid kernel");

    Kernel* kernel = kitr->second;
    if(kernel->ready()){
      return;
    }

    CommonData* commonData = kernel->commonData();
    CommonField* commonField = commonData->getField(fieldName);
    if(!commonField){
      commonField = commonData->addField(fieldName, hostPtr,
                                         elementSize, size); 
    }
    
    kernel->addField(fieldName, commonField, mode);
  }

  void runKernel(const char* kernelName){
    auto kitr = kernelMap_.find(kernelName);
    assert(kitr != kernelMap_.end() && "invalid kernel");

    Kernel* kernel = kitr->second;
    kernel->run();
  }

private:
  typedef map<const char*, PTXModule*> ModuleMap_;
  typedef map<const char*, CommonData*> MeshMap_;
  typedef map<const char*, Kernel*> KernelMap_;

  CUdevice device_;
  CUcontext context_;
  size_t numThreads_;

  ModuleMap_ moduleMap_;
  KernelMap_ kernelMap_;
};

CUDARuntime::Kernel* 
CUDARuntime::PTXModule::createKernel(CommonData* commonData,
                                     const char* kernelName){
  CUfunction function;
  CUresult err = cuModuleGetFunction(&function, module_, kernelName);
  check(err);
  
  Kernel* kernel = new Kernel(this, commonData, function);
  
  return kernel;
}

extern "C" {

  void __kitsune_cuda_init(){
    CUDARuntime* runtime = new CUDARuntime;
    runtime->init();

    GPURuntime::init(runtime);
  }

} // extern "C"
