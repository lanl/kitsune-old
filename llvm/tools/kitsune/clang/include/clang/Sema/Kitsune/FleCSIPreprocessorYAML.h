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
// Vararg-related helper classes + YAML mappings
// -----------------------------------------------------------------------------

namespace flecsi {

// FleCSIVarArgType
struct FleCSIVarArgType {
   std::string type;
   FleCSIVarArgType() : type("") { }
   FleCSIVarArgType(const std::string &_type)
    : type(_type)
   { }
};

// FleCSIVarArgTypeValue
struct FleCSIVarArgTypeValue {
   std::string type;
   std::string value;
   FleCSIVarArgTypeValue() : type(""), value("") { }
   FleCSIVarArgTypeValue(const std::string &_type, const std::string &_value)
    : type (_type),
      value(_value)
   { }
};

}

namespace llvm {
namespace yaml {

template<>
struct MappingTraits<flecsi::FleCSIVarArgType> {
   static void mapping(IO &io, flecsi::FleCSIVarArgType &c)
   {
      kitsune_map(type);
   }
};

template<>
struct MappingTraits<flecsi::FleCSIVarArgTypeValue> {
   static void mapping(IO &io, flecsi::FleCSIVarArgTypeValue &c)
   {
      kitsune_map(type);
      kitsune_map(value);
   }
};

}
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
// From FleCSI's execution.h
// Task Registration
// -----------------------------------------------------------------------------

// flecsi_register_task_simple ( task, processor, launch )
kitsune_makeclass(FleCSIRegisterTaskSimple, flecsi_register_task_simple)
   std::string task;
   std::string processor;
   std::string launch;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterTaskSimple)
   kitsune_map(task);
   kitsune_map(processor);
   kitsune_map(launch);
kitsune_maptraits_done


// flecsi_register_task ( task, nspace, processor, launch )
kitsune_makeclass(FleCSIRegisterTask, flecsi_register_task)
   std::string task;
   std::string nspace;
   std::string processor;
   std::string launch;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterTask)
   kitsune_map(task);
   kitsune_map(nspace);
   kitsune_map(processor);
   kitsune_map(launch);
kitsune_maptraits_done


// flecsi_register_mpi_task_simple ( task )
kitsune_makeclass(FleCSIRegisterMPITaskSimple, flecsi_register_mpi_task_simple)
   std::string task;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterMPITaskSimple)
   kitsune_map(task);
kitsune_maptraits_done


// flecsi_register_mpi_task ( task, nspace )
kitsune_makeclass(FleCSIRegisterMPITask, flecsi_register_mpi_task)
   std::string task;
   std::string nspace;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterMPITask)
   kitsune_map(task);
   kitsune_map(nspace);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Task Execution
// -----------------------------------------------------------------------------

// flecsi_execute_task_simple ( task, launch, ... )
kitsune_makeclass(FleCSIExecuteTaskSimple, flecsi_execute_task_simple)
   std::string task;
   std::string launch;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIExecuteTaskSimple)
   kitsune_map(task);
   kitsune_map(launch);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_execute_task ( task, nspace, launch, ... )
kitsune_makeclass(FleCSIExecuteTask, flecsi_execute_task)
   std::string task;
   std::string nspace;
   std::string launch;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIExecuteTask)
   kitsune_map(task);
   kitsune_map(nspace);
   kitsune_map(launch);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_execute_mpi_task_simple ( task, ... )
kitsune_makeclass(FleCSIExecuteMPITaskSimple, flecsi_execute_mpi_task_simple)
   std::string task;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIExecuteMPITaskSimple)
   kitsune_map(task);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_execute_mpi_task ( task, nspace, ... )
kitsune_makeclass(FleCSIExecuteMPITask, flecsi_execute_mpi_task)
   std::string task;
   std::string nspace;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIExecuteMPITask)
   kitsune_map(task);
   kitsune_map(nspace);
   kitsune_map(varargs);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// *_global_object
// -----------------------------------------------------------------------------

// flecsi_register_global_object ( index, nspace, type )
kitsune_makeclass(FleCSIRegisterGlobalObject, flecsi_register_global_object)
   std::string index;
   std::string nspace;
   std::string type;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
kitsune_maptraits_done


// flecsi_set_global_object ( index, nspace, type, obj )
kitsune_makeclass(FleCSISetGlobalObject, flecsi_set_global_object)
   std::string index;
   std::string nspace;
   std::string type;
   std::string obj;
kitsune_makeclass_done
kitsune_maptraits(FleCSISetGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
   kitsune_map(obj);
kitsune_maptraits_done


// flecsi_initialize_global_object ( index, nspace, type, ... )
kitsune_makeclass(FleCSIInitializeGlobalObject, flecsi_initialize_global_object)
   std::string index;
   std::string nspace;
   std::string type;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIInitializeGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_get_global_object ( index, nspace, type )
kitsune_makeclass(FleCSIGetGlobalObject, flecsi_get_global_object)
   std::string index;
   std::string nspace;
   std::string type;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetGlobalObject)
   kitsune_map(index);
   kitsune_map(nspace);
   kitsune_map(type);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Function Interface
// -----------------------------------------------------------------------------

// flecsi_register_function ( func, nspace )
kitsune_makeclass(FleCSIRegisterFunction, flecsi_register_function)
   std::string func;
   std::string nspace;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterFunction)
   kitsune_map(func);
   kitsune_map(nspace);
kitsune_maptraits_done


// flecsi_execute_function ( handle, ... )
kitsune_makeclass(FleCSIExecuteFunction, flecsi_execute_function)
   std::string handle;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIExecuteFunction)
   kitsune_map(handle);
   kitsune_map(varargs);
kitsune_maptraits_done


// flecsi_function_handle ( func, nspace )
kitsune_makeclass(FleCSIFunctionHandle, flecsi_function_handle)
   std::string func;
   std::string nspace;
kitsune_makeclass_done
kitsune_maptraits(FleCSIFunctionHandle)
   kitsune_map(func);
   kitsune_map(nspace);
kitsune_maptraits_done


// flecsi_define_function_type ( func, return_type, ... )
kitsune_makeclass(FleCSIDefineFunctionType, flecsi_define_function_type)
   std::string func;
   std::string return_type;
   std::vector<FleCSIVarArgType> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIDefineFunctionType)
   kitsune_map(func);
   kitsune_map(return_type);
   kitsune_map(varargs);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Registration
// -----------------------------------------------------------------------------

// flecsi_register_data_client ( client_type, nspace, name )
kitsune_makeclass(FleCSIRegisterDataClient, flecsi_register_data_client)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterDataClient)
   kitsune_map(client_type);
   kitsune_map(nspace);
   kitsune_map(name);
kitsune_maptraits_done


// flecsi_register_field
//    ( client_type, nspace, name, data_type, storage_class, versions, ...)
kitsune_makeclass(FleCSIRegisterField, flecsi_register_field)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t versions;
   std::uint32_t index_space;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterField)
   kitsune_map(client_type);
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(versions);
   kitsune_map(index_space);
kitsune_maptraits_done


// flecsi_register_global ( nspace, name, data_type, versions)
kitsune_makeclass(FleCSIRegisterGlobal, flecsi_register_global)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t versions;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterGlobal)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(versions);
kitsune_maptraits_done


// flecsi_register_color ( nspace, name, data_type, versions)
kitsune_makeclass(FleCSIRegisterColor, flecsi_register_color)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t versions;
kitsune_makeclass_done
kitsune_maptraits(FleCSIRegisterColor)
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
kitsune_makeclass(FleCSIGetHandle, flecsi_get_handle)
   std::string   client_handle;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t version;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetHandle)
   kitsune_map(client_handle);
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
kitsune_maptraits_done


// flecsi_get_client_handle ( client_type, nspace, name )
kitsune_makeclass(FleCSIGetClientHandle, flecsi_get_client_handle)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetClientHandle)
   kitsune_map(client_type);
   kitsune_map(nspace);
   kitsune_map(name);
kitsune_maptraits_done



/*
The following appear to be either deprecated, or not yet implemented.
They both insert calls to
   flecsi::data::field_interface_t::get_handles<>()
which in turn forward to purported get_handles() functions in DATA_POLICY::.
However, no DATA_POLICY::get_handles() functions are actually defined in
any data policy classes I can find. So, I won't implement these, until and
unless I determine that they're relevant and wanted.

// flecsi_get_handles ( client, nspace, data_type, storage_class, version, ... )
kitsune_makeclass(FleCSIGetHandles, flecsi_get_handles)
   std::string   client;
   std::string   nspace;
   std::string   data_type;
   std::string   storage_class;
   std::string   version;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetHandles)
   kitsune_map(client);
   kitsune_map(nspace);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
   kitsune_map(varargs);
kitsune_maptraits_done

// flecsi_get_handles_all ( client, data_type, storage_class, version, ... )
kitsune_makeclass(FleCSIGetHandlesAll, flecsi_get_handles_all)
   std::string   client;
   std::string   data_type;
   std::string   storage_class;
   std::string   version;
   std::vector<FleCSIVarArgTypeValue> varargs;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetHandlesAll)
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
kitsune_makeclass(FleCSIGetMutator, flecsi_get_mutator)
   std::string   client_handle;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t version;
   std::string   slots;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetMutator)
   kitsune_map(client_handle);
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(storage_class);
   kitsune_map(version);
   kitsune_map(slots);
kitsune_maptraits_done


// flecsi_get_global ( nspace, name, data_type, version )
kitsune_makeclass(FleCSIGetGlobal, flecsi_get_global)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t version;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetGlobal)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(version);
kitsune_maptraits_done


// flecsi_get_color ( nspace, name, data_type, version )
kitsune_makeclass(FleCSIGetColor, flecsi_get_color)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t version;
kitsune_makeclass_done
kitsune_maptraits(FleCSIGetColor)
   kitsune_map(nspace);
   kitsune_map(name);
   kitsune_map(data_type);
   kitsune_map(version);
kitsune_maptraits_done



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Miscellaneous macros.
// -----------------------------------------------------------------------------

/*
flecsi_has_attribute    ( attribute)
flecsi_is_at            (            index_space )
flecsi_has_attribute_at ( attribute, index_space )

flecsi_put_all_handles
( client, storage_class, num_handles, handles, hashes, namespaces, versions )
flecsi_get_all_handles
( client, storage_class,              handles, hashes, namespaces, versions )

Apparently, these are deprecated; so, we won't implement them at this time.
The first three are used only in the flecsi/data/test/storage_class.cc, but
I noticed while working elsewhere that this test isn't currently included in
the test suite. As for the flecsi_[put|get]_all_handles macros, they aren't
called from anywhere in the current FleCSI code. Also, they insert code with
calls to function templates "put_all_handles" and "get_all_handles"; those
are nowhere to be found in FleCSI.
*/



// -----------------------------------------------------------------------------
// PreprocessorYAML
// MappingTraits<PreprocessorYAML>
// -----------------------------------------------------------------------------

namespace flecsi {

class PreprocessorYAML {
public:

   #define kitsune_vector(name) std::vector<name> name

   kitsune_vector(FleCSIRegisterTaskSimple);
   kitsune_vector(FleCSIRegisterTask);
   kitsune_vector(FleCSIRegisterMPITaskSimple);
   kitsune_vector(FleCSIRegisterMPITask);

   kitsune_vector(FleCSIExecuteTaskSimple);
   kitsune_vector(FleCSIExecuteTask);
   kitsune_vector(FleCSIExecuteMPITaskSimple);
   kitsune_vector(FleCSIExecuteMPITask);

   kitsune_vector(FleCSIRegisterGlobalObject);
   kitsune_vector(FleCSISetGlobalObject);
   kitsune_vector(FleCSIInitializeGlobalObject);
   kitsune_vector(FleCSIGetGlobalObject);

   kitsune_vector(FleCSIRegisterDataClient);
   kitsune_vector(FleCSIRegisterField);
   kitsune_vector(FleCSIRegisterGlobal);
   kitsune_vector(FleCSIRegisterColor);

   kitsune_vector(FleCSIRegisterFunction);
   kitsune_vector(FleCSIExecuteFunction);
   kitsune_vector(FleCSIFunctionHandle);
   kitsune_vector(FleCSIDefineFunctionType);

   kitsune_vector(FleCSIGetHandle);
   kitsune_vector(FleCSIGetClientHandle);
   // kitsune_vector(FleCSIGetHandles);
   // kitsune_vector(FleCSIGetHandlesAll);

   kitsune_vector(FleCSIGetMutator);
   kitsune_vector(FleCSIGetGlobal);
   kitsune_vector(FleCSIGetColor);
};

}



namespace llvm {
namespace yaml {

template<>
class MappingTraits<flecsi::PreprocessorYAML> {
public:

   static void mapping(IO &io, flecsi::PreprocessorYAML &c)
   {
      kitsune_map(FleCSIRegisterTaskSimple);
      kitsune_map(FleCSIRegisterTask);
      kitsune_map(FleCSIRegisterMPITaskSimple);
      kitsune_map(FleCSIRegisterMPITask);

      kitsune_map(FleCSIExecuteTaskSimple);
      kitsune_map(FleCSIExecuteTask);
      kitsune_map(FleCSIExecuteMPITaskSimple);
      kitsune_map(FleCSIExecuteMPITask);

      kitsune_map(FleCSIRegisterGlobalObject);
      kitsune_map(FleCSISetGlobalObject);
      kitsune_map(FleCSIInitializeGlobalObject);
      kitsune_map(FleCSIGetGlobalObject);

      kitsune_map(FleCSIRegisterDataClient);
      kitsune_map(FleCSIRegisterField);
      kitsune_map(FleCSIRegisterGlobal);
      kitsune_map(FleCSIRegisterColor);

      kitsune_map(FleCSIRegisterFunction);
      kitsune_map(FleCSIExecuteFunction);
      kitsune_map(FleCSIFunctionHandle);
      kitsune_map(FleCSIDefineFunctionType);

      kitsune_map(FleCSIGetHandle);
      kitsune_map(FleCSIGetClientHandle);
      //kitsune_map(FleCSIGetHandles);
      //kitsune_map(FleCSIGetHandlesAll);

      kitsune_map(FleCSIGetMutator);
      kitsune_map(FleCSIGetGlobal);
      kitsune_map(FleCSIGetColor);
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
