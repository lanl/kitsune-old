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

#ifndef FleCSIPreprocessorYAML
#define FleCSIPreprocessorYAML

#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/Support/YAMLTraits.h"

#include "clang/Sema/Kitsune/FleCSIMisc.h"



// -----------------------------------------------------------------------------
// Helper macros
// -----------------------------------------------------------------------------

#define kitsune_makeclass(classname,macroname) \
   namespace flecsi { \
      struct classname : public macrobase { \
         classname() \
          : macrobase(#macroname) { } \
         classname(const clang::Sema &sema, const clang::Token &token) \
          : macrobase(#macroname, sema, token) \
         { }

#define kitsune_makeclass_done \
      }; \
   }



#define kitsune_maptraits(classname) \
   namespace llvm { \
   namespace yaml { \
      template<> \
      struct MappingTraits<flecsi::classname> { \
         static void mapping(IO &io, flecsi::classname &c) \
         { \
            c.map(io);

#define kitsune_map(field) \
            io.mapRequired(#field, c.field)

#define kitsune_maptraits_done \
         } \
      }; \
   } \
   }



// -----------------------------------------------------------------------------
// macrobase
// -----------------------------------------------------------------------------

namespace flecsi {

class macrobase {
public:
   // data
   std::string   macro;  // macro name
   std::string   file;   // file name
   std::uint32_t line;   // line number

   // ctor: name
   // File and line are defaulted
   macrobase(const std::string &name)
    : macro(name), file(""), line(0)
   { }

   // ctor: name, sema, token
   // The last two give file and line
   macrobase(
      const std::string  &name,
      const clang::Sema  &sema,
      const clang::Token &token
   )
    : macro(name)
   {
      auto pair = getFileLine(sema,token);
      file = pair.first;
      line = pair.second;
   }

   // map
   // Stipulate mapRequired for all of macrobase's data
   void map(llvm::yaml::IO &io)
   {
      io.mapRequired("macro", macro);
      io.mapRequired("file",  file);
      io.mapRequired("line",  line);
   }
};

}



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// For several classes X:
//    X
//    MappingTraits<X>
// Every such pair corresponds to some FleCSI macro.
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Vararg helper constructs
// -----------------------------------------------------------------------------

namespace flecsi {

// FlecsiVarArgType
struct FlecsiVarArgType {
   std::string type;
   FlecsiVarArgType() : type("") { }
   FlecsiVarArgType(const std::string &_type)
    : type(_type)
   { }
};

// FlecsiVarArgTypeValue
struct FlecsiVarArgTypeValue {
   std::string type;
   std::string value;
   FlecsiVarArgTypeValue() : type(""), value("") { }
   FlecsiVarArgTypeValue(const std::string &_type, const std::string &_value)
    : type (_type),
      value(_value)
   { }
};

}

namespace llvm {
namespace yaml {

template<>
struct MappingTraits<flecsi::FlecsiVarArgType> {
   static void mapping(IO &io, flecsi::FlecsiVarArgType &c)
   {
      kitsune_map(type);
   }
};

template<>
struct MappingTraits<flecsi::FlecsiVarArgTypeValue> {
   static void mapping(IO &io, flecsi::FlecsiVarArgTypeValue &c)
   {
      kitsune_map(type);
      kitsune_map(value);
   }
};

}
}



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Task Registration
// -----------------------------------------------------------------------------

// flecsi_register_task_simple ( task, processor, launch )
kitsune_makeclass(FlecsiRegisterTaskSimple, flecsi_register_task_simple)
   std::string task;
   std::string processor;
   std::string launch;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterTaskSimple)
   kitsune_map(task);
   kitsune_map(processor);
   kitsune_map(launch);
kitsune_maptraits_done


// flecsi_register_task ( task, nspace, processor, launch )
kitsune_makeclass(FlecsiRegisterTask, flecsi_register_task)
   std::string task;
   std::string nspace;
   std::string processor;
   std::string launch;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterTask)
   kitsune_map(task);
   kitsune_map(nspace);
   kitsune_map(processor);
   kitsune_map(launch);
kitsune_maptraits_done


// flecsi_register_mpi_task_simple ( task )
kitsune_makeclass(FlecsiRegisterMPITaskSimple, flecsi_register_mpi_task_simple)
   std::string task;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterMPITaskSimple)
   kitsune_map(task);
kitsune_maptraits_done


// flecsi_register_mpi_task ( task, nspace )
kitsune_makeclass(FlecsiRegisterMPITask, flecsi_register_mpi_task)
   std::string task;
   std::string nspace;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterMPITask)
   kitsune_map(task);
   kitsune_map(nspace);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Task Execution
// -----------------------------------------------------------------------------

// flecsi_execute_task_simple ( task, launch, ... )
kitsune_makeclass(FlecsiExecuteTaskSimple, flecsi_execute_task_simple)
   std::string task;
   std::string launch;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiExecuteTaskSimple)
   kitsune_map(task);
   kitsune_map(launch);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_execute_task ( task, nspace, launch, ... )
kitsune_makeclass(FlecsiExecuteTask, flecsi_execute_task)
   std::string task;
   std::string nspace;
   std::string launch;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiExecuteTask)
   kitsune_map(task);
   kitsune_map(nspace);
   kitsune_map(launch);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_execute_mpi_task_simple ( task, ... )
kitsune_makeclass(FlecsiExecuteMPITaskSimple, flecsi_execute_mpi_task_simple)
   std::string task;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiExecuteMPITaskSimple)
   kitsune_map(task);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_execute_mpi_task ( task, nspace, ... )
kitsune_makeclass(FlecsiExecuteMPITask, flecsi_execute_mpi_task)
   std::string task;
   std::string nspace;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiExecuteMPITask)
   kitsune_map(task);
   kitsune_map(nspace);
   kitsune_map(varargs);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// *_global_object
// -----------------------------------------------------------------------------

// flecsi_register_global_object ( index, nspace, type )
kitsune_makeclass(FlecsiRegisterGlobalObject, flecsi_register_global_object)
   std::string index;
   std::string nspace;
   std::string type;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
kitsune_maptraits_done


// flecsi_set_global_object ( index, nspace, type, obj )
kitsune_makeclass(FlecsiSetGlobalObject, flecsi_set_global_object)
   std::string index;
   std::string nspace;
   std::string type;
   std::string obj;
kitsune_makeclass_done
kitsune_maptraits(FlecsiSetGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
   kitsune_map(obj);
kitsune_maptraits_done


// flecsi_initialize_global_object ( index, nspace, type, ... )
kitsune_makeclass(FlecsiInitializeGlobalObject, flecsi_initialize_global_object)
   std::string index;
   std::string nspace;
   std::string type;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiInitializeGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_get_global_object ( index, nspace, type )
kitsune_makeclass(FlecsiGetGlobalObject, flecsi_get_global_object)
   std::string index;
   std::string nspace;
   std::string type;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Function Interface
// -----------------------------------------------------------------------------

// flecsi_register_function ( func, nspace )
kitsune_makeclass(FlecsiRegisterFunction, flecsi_register_function)
   std::string func;
   std::string nspace;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterFunction)
   kitsune_map(func);
   kitsune_map(nspace);
kitsune_maptraits_done


// flecsi_execute_function ( handle, ... )
kitsune_makeclass(FlecsiExecuteFunction, flecsi_execute_function)
   std::string handle;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiExecuteFunction)
   kitsune_map(handle);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_function_handle ( func, nspace )
kitsune_makeclass(FlecsiFunctionHandle, flecsi_function_handle)
   std::string func;
   std::string nspace;
kitsune_makeclass_done
kitsune_maptraits(FlecsiFunctionHandle)
   kitsune_map(func);
   kitsune_map(nspace);
kitsune_maptraits_done


// flecsi_define_function_type ( func, return_type, ... )
kitsune_makeclass(FlecsiDefineFunctionType, flecsi_define_function_type)
   std::string func;
   std::string return_type;
   std::vector<FlecsiVarArgType> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiDefineFunctionType)
   kitsune_map(func);
   kitsune_map(return_type);
   kitsune_map(varargs);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Registration
// -----------------------------------------------------------------------------

// flecsi_register_data_client ( client_type, nspace, name )
kitsune_makeclass(FlecsiRegisterDataClient, flecsi_register_data_client)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterDataClient)
   kitsune_map(client_type);
   kitsune_map(nspace);
   kitsune_map(name);
kitsune_maptraits_done


// flecsi_register_field
//    ( client_type, nspace, name, data_type, storage_class, versions, ...)
kitsune_makeclass(FlecsiRegisterField, flecsi_register_field)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t versions;
   std::uint32_t index_space;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterField)
   kitsune_map(client_type);
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(versions);
   kitsune_map(index_space);
kitsune_maptraits_done


// flecsi_register_global ( nspace, name, data_type, versions)
kitsune_makeclass(FlecsiRegisterGlobal, flecsi_register_global)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t versions;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterGlobal)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(versions);
kitsune_maptraits_done


// flecsi_register_color ( nspace, name, data_type, versions)
kitsune_makeclass(FlecsiRegisterColor, flecsi_register_color)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t versions;
kitsune_makeclass_done
kitsune_maptraits(FlecsiRegisterColor)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(versions);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Some handle-related macros.
// -----------------------------------------------------------------------------

// flecsi_get_handle
//    ( client_handle, nspace, name, data_type, storage_class, version )
kitsune_makeclass(FlecsiGetHandle, flecsi_get_handle)
   std::string   client_handle;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t version;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetHandle)
   kitsune_map(client_handle);
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
kitsune_maptraits_done


// flecsi_get_client_handle ( client_type, nspace, name )
kitsune_makeclass(FlecsiGetClientHandle, flecsi_get_client_handle)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetClientHandle)
   kitsune_map(client_type);
   kitsune_map(nspace);
   kitsune_map(name);
kitsune_maptraits_done



// The following appear to be either deprecated or not not-yet-implemented.
// They both insert calls to
//    flecsi::data::field_interface_t::get_handles<>()
// which in turn forward to purported get_handles() functions in DATA_POLICY::.
// However, no DATA_POLICY::get_handles() functions are actually defined in
// any data policy classes I can find. So, I won't implement those two here,
// until and unless I determine that they're relevant and wanted.

/*
// flecsi_get_handles ( client, nspace, data_type, storage_class, version, ... )
kitsune_makeclass(FlecsiGetHandles, flecsi_get_handles)
   std::string   client;
   std::string   nspace;
   std::string   data_type;
   std::string   storage_class;
   std::string   version;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetHandles)
   kitsune_map(client);
   kitsune_map(nspace);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_get_handles_all ( client, data_type, storage_class, version, ... )
kitsune_makeclass(FlecsiGetHandlesAll, flecsi_get_handles_all)
   std::string   client;
   std::string   data_type;
   std::string   storage_class;
   std::string   version;
   std::vector<FlecsiVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetHandlesAll)
   kitsune_map(client);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
   kitsune_map(varargs);
kitsune_maptraits_done
*/



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Some flecsi_get_* macros.
// -----------------------------------------------------------------------------

// flecsi_get_mutator
//    ( client_handle, nspace, name, data_type, storage_class, version, slots )
kitsune_makeclass(FlecsiGetMutator, flecsi_get_mutator)
   std::string   client_handle;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t version;
   std::string   slots;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetMutator)
   kitsune_map(client_handle);
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
   kitsune_map(slots);
kitsune_maptraits_done


// flecsi_get_global ( nspace, name, data_type, version )
kitsune_makeclass(FlecsiGetGlobal, flecsi_get_global)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t version;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetGlobal)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(version);
kitsune_maptraits_done


// flecsi_get_color ( nspace, name, data_type, version )
kitsune_makeclass(FlecsiGetColor, flecsi_get_color)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t version;
kitsune_makeclass_done
kitsune_maptraits(FlecsiGetColor)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(version);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Miscellaneous macros.
// -----------------------------------------------------------------------------

// flecsi_has_attribute    ( attribute)
// flecsi_is_at            (            index_space )
// flecsi_has_attribute_at ( attribute, index_space )
//
// flecsi_put_all_handles
// ( client, storage_class, num_handles, handles, hashes, namespaces, versions )
// flecsi_get_all_handles
// ( client, storage_class,              handles, hashes, namespaces, versions )
//
// In fact, it appears that these are all deprecated, so I won't implement any
// processing of those macros, unless I determine otherwise. The first three
// are used only in the flecsi/data/test/storage_class.cc; but I noticed while
// working elsewhere that that test doesn't currently seem to be included in
// the test suite. As for the flecsi_[put|get]_all_handles macros, they aren't
// called from anywhere in the current FleCSI code. Also, they insert code with
// calls to function templates called put_all_handles and get_all_handles; those
// are nowhere to be found in FleCSI.



// -----------------------------------------------------------------------------
// PreprocessorYAML
// MappingTraits<PreprocessorYAML>
// -----------------------------------------------------------------------------

namespace flecsi {

class PreprocessorYAML {
public:

   #define kitsune_vector(name) std::vector<name> name

   kitsune_vector(FlecsiRegisterTaskSimple);
   kitsune_vector(FlecsiRegisterTask);
   kitsune_vector(FlecsiRegisterMPITaskSimple);
   kitsune_vector(FlecsiRegisterMPITask);

   kitsune_vector(FlecsiExecuteTaskSimple);
   kitsune_vector(FlecsiExecuteTask);
   kitsune_vector(FlecsiExecuteMPITaskSimple);
   kitsune_vector(FlecsiExecuteMPITask);

   kitsune_vector(FlecsiRegisterGlobalObject);
   kitsune_vector(FlecsiSetGlobalObject);
   kitsune_vector(FlecsiInitializeGlobalObject);
   kitsune_vector(FlecsiGetGlobalObject);

   kitsune_vector(FlecsiRegisterDataClient);
   kitsune_vector(FlecsiRegisterField);
   kitsune_vector(FlecsiRegisterGlobal);
   kitsune_vector(FlecsiRegisterColor);

   kitsune_vector(FlecsiRegisterFunction);
   kitsune_vector(FlecsiExecuteFunction);
   kitsune_vector(FlecsiFunctionHandle);
   kitsune_vector(FlecsiDefineFunctionType);

   kitsune_vector(FlecsiGetHandle);
   kitsune_vector(FlecsiGetClientHandle);
   //kitsune_vector(FlecsiGetHandles);
   //kitsune_vector(FlecsiGetHandlesAll);

   kitsune_vector(FlecsiGetMutator);
   kitsune_vector(FlecsiGetGlobal);
   kitsune_vector(FlecsiGetColor);
};

}



namespace llvm {
namespace yaml {

template<>
class MappingTraits<flecsi::PreprocessorYAML> {
public:

   static void mapping(IO &io, flecsi::PreprocessorYAML &c)
   {
      kitsune_map(FlecsiRegisterTaskSimple);
      kitsune_map(FlecsiRegisterTask);
      kitsune_map(FlecsiRegisterMPITaskSimple);
      kitsune_map(FlecsiRegisterMPITask);

      kitsune_map(FlecsiExecuteTaskSimple);
      kitsune_map(FlecsiExecuteTask);
      kitsune_map(FlecsiExecuteMPITaskSimple);
      kitsune_map(FlecsiExecuteMPITask);

      kitsune_map(FlecsiRegisterGlobalObject);
      kitsune_map(FlecsiSetGlobalObject);
      kitsune_map(FlecsiInitializeGlobalObject);
      kitsune_map(FlecsiGetGlobalObject);

      kitsune_map(FlecsiRegisterDataClient);
      kitsune_map(FlecsiRegisterField);
      kitsune_map(FlecsiRegisterGlobal);
      kitsune_map(FlecsiRegisterColor);

      kitsune_map(FlecsiRegisterFunction);
      kitsune_map(FlecsiExecuteFunction);
      kitsune_map(FlecsiFunctionHandle);
      kitsune_map(FlecsiDefineFunctionType);

      kitsune_map(FlecsiGetHandle);
      kitsune_map(FlecsiGetClientHandle);
      //kitsune_map(FlecsiGetHandles);
      //kitsune_map(FlecsiGetHandlesAll);

      kitsune_map(FlecsiGetMutator);
      kitsune_map(FlecsiGetGlobal);
      kitsune_map(FlecsiGetColor);
   }
};

}
}

// cleanup
#undef kitsune_makeclass
#undef kitsune_makeclass_done
#undef kitsune_maptraits
#undef kitsune_map
#undef kitsune_maptraits_done
#undef kitsune_vector

#endif
