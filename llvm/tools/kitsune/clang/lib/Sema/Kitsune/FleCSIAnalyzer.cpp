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

#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "clang/Sema/Kitsune/FleCSIAnalyzer.h"
#include "llvm/Support/YAMLTraits.h"
#include <fstream>



// -----------------------------------------------------------------------------
// Helper constructs for debugging
// -----------------------------------------------------------------------------

#define noKITSUNE_DEBUG

// includes
#ifdef KITSUNE_DEBUG
   #include <iostream>
   #include <typeinfo>
   #include <boost/core/demangle.hpp>
#endif

// kitsune_print
#ifdef KITSUNE_DEBUG
   #define kitsune_print(x) \
   std::cout << "kitsune: " #x " == " << (x) << std::endl
#else
   #define kitsune_print(x)
#endif



namespace {

// kitsune_debug
inline void kitsune_debug(const std::string &s, const bool newline = true)
{
   #ifdef KITSUNE_DEBUG
      std::cout << "kitsune: " << s;
      if (newline)
         std::cout << std::endl;
   #else
      (void)s;
      (void)newline;
   #endif
}

/*
// print_type(mangled_name)
inline void print_type(const char *const mangled_name)
{
   #ifdef KITSUNE_DEBUG
      kitsune_debug(boost::core::demangle(mangled_name));
   #else
      (void)mangled_name;
   #endif
}

// print_type<T>()
template<class T>
inline void print_type()
{
   #ifdef KITSUNE_DEBUG
      print_type(typeid(T).name());
   #endif
}
*/

} // namespace



// -----------------------------------------------------------------------------
// Helper constructs
//    tostr
//    getFileLine
//    MacroData
//    greatest_less_or_equal
// Unnamed namespace ("static"); so constructs are used only in this file.
// -----------------------------------------------------------------------------

namespace {

// tostr
// Get a token's spelling.
std::string tostr(
   const clang::Token &token,
   const clang::Sema  &sema
) {
   kitsune_debug("tostr()");
   return clang::Lexer::getSpelling(
      token,
      sema.getSourceManager(),
      sema.getLangOpts()
   );
}



// getFileLine
// Get a token's file name and line number.
std::pair<std::string,std::uint32_t> getFileLine(
   const clang::Token &token,
   const clang::Sema  &sema
) {
   kitsune_debug("getFileLine()");

   clang::SourceManager &srcMgr = sema.getSourceManager();
   clang::SourceLocation loc = token.getLocation();

   auto p = srcMgr.getDecomposedLoc(loc);
   return std::pair<std::string,std::uint32_t>(
      srcMgr.getFilename(loc).str(),
      srcMgr.getLineNumber(p.first, p.second)
   );
}



// MacroData
struct MacroData
{
   // Token representing the macro name itself
   clang::Token tok;

   // Offset of the macro invocation's end, i.e. the ")" in "macro(foo,bar,...)"
   std::size_t end;

   // Arguments
   // Each argument consists of some number of tokens (often just one).
   std::vector<std::vector<clang::Token>> args;

   // Number of arguments to the macro
   inline std::size_t size() const
   {
      return args.size();
   }

   // Location of argument [a], token [t].
   inline clang::SourceLocation loc(
      const std::size_t a,
      const std::size_t t
   ) const {
      kitsune_debug("MacroData::loc(a,t)");
      assert(a < args.size());
      assert(t < args[a].size());
      return args[a][t].getLocation();
   }

   // String representation of argument [a], token [t]
   inline std::string str(
      const std::size_t a,
      const std::size_t t,
      clang::Sema &sema
   ) const {
      kitsune_debug("MacroData::str(a,t)");
      assert(a < args.size());
      assert(t < args[a].size());
      return tostr(args[a][t],sema);
   }

   // String representation of argument [a] (all tokens put together)
   inline std::string str(
      const std::size_t a,
      clang::Sema &sema
   ) const {
      kitsune_debug("MacroData::str(a)");
      assert(a < args.size());
      std::string s;
      for (std::size_t t = 0;  t < args[a].size();  ++t)
         s += (t ? " " : "") + str(a,t,sema);
      return s;
   }
};



// greatest_less_or_equal
template<class MAP>
typename MAP::const_iterator
greatest_less_or_equal(
   const MAP &m,
   const typename MAP::key_type &k
) {
    typename MAP::const_iterator it = m.upper_bound(k);
    return it == m.begin() ? m.end() : --it;
}

template<class MAP>
typename MAP::iterator
greatest_less_or_equal(
   MAP &m,
   const typename MAP::key_type &k
) {
    typename MAP::iterator it = m.upper_bound(k);
    return it == m.begin() ? m.end() : --it;
}

} // namespace



// -----------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------

// getTemplateArgs
const clang::TemplateArgumentList *getTemplateArgs(
   const clang::FunctionDecl *const fd
) {
   kitsune_debug("getTemplateArgs()");

   return fd->getTemplateSpecializationArgs();
}



// getClassDecl
const clang::CXXRecordDecl *getClassDecl(clang::QualType qt)
{
   kitsune_debug("getClassDecl()");

   const clang::Type *t = qt.getTypePtr();

   const clang::RecordType *rt = clang::dyn_cast<clang::RecordType>(t);
   if (!rt)
      return nullptr;

   const clang::RecordDecl *rd = rt->getDecl();
   const clang::CXXRecordDecl *cd = clang::dyn_cast<clang::CXXRecordDecl>(rd);
   if (!cd)
      return nullptr;

   return cd;
}



// getMethod
const clang::CXXMethodDecl *
getMethod(const clang::CallExpr *const call)
{
   kitsune_debug("getMethod()");

   const clang::Decl *cd = call->getCalleeDecl();
   if (!cd)
      return nullptr;

   auto md = clang::dyn_cast<clang::CXXMethodDecl>(cd);
   if (!md)
      return nullptr;

   return md;
}



// getIntArg
std::int64_t getIntArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("getIntArg()");
   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getSExtValue();
}

// getUIntArg
std::uint64_t getUIntArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("getUIntArg()");
   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getZExtValue();
}

// getTypeArg
clang::QualType getTypeArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("getTypeArg()");
   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Type);
   return arg.getAsType();
}



// -----------------------------------------------------------------------------
// flecsi_base
// Helper.
// Has macro, file, line.
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

struct flecsi_base
{
   std::string   macro;
   std::string   file;
   std::uint32_t line;

   flecsi_base(
      const std::string &_macro,
      const clang::Sema *const sema = nullptr,
      const MacroData   *const data = nullptr
   )
    : macro(_macro)
   {
      if (sema && data) {
         auto pair = getFileLine(data->tok, *sema);
         file = pair.first;
         line = pair.second;
      } else {
         file = "";
         line = 0;
      }
   }

   void boilerplate(IO &io)
   {
      io.mapRequired("macro", macro);
      io.mapRequired("file",  file);
      io.mapRequired("line",  line);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// SequenceTraits<vector<T>>
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

template<class T, class ALLOCATOR>
struct SequenceTraits<std::vector<T,ALLOCATOR>>
{
   static std::size_t size(
      IO &,
      std::vector<T,ALLOCATOR> &vec
   ) {
      kitsune_debug("size()");
      return vec.size();
   }

   static T &element(
      IO &,
      std::vector<T,ALLOCATOR> &vec,
      const std::size_t index
   ) {
      kitsune_debug("element()");
      return vec[index];
   }

   /*
   static const T &element(
      IO &,
      const std::vector<T,ALLOCATOR> &vec,
      const std::size_t index
   ) {
      kitsune_debug("element()");
      return vec[index];
   }
   */
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// For several classes X:
//    X
//    MappingTraits<X>
// Then, all together:
//    FleCSIMetadata
//    MappingTraits<FleCSIMetadata>
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Helper macros

#define kitsune_macro_ctor(class_name, macro_name) \
   class_name( \
      const clang::Sema *const sema = nullptr, \
      const MacroData   *const data = nullptr  \
   ) : \
      flecsi_base(#macro_name, sema, data) \
   { }

#define kitsune_map(field) io.mapRequired(#field, c.field);



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Task Registration
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_register_task_simple     ( task,         processor, launch )
// flecsi_register_task            ( task, nspace, processor, launch )
// flecsi_register_mpi_task_simple ( task                            )
// flecsi_register_mpi_task        ( task, nspace                    )



// flecsi_register_task_simple
struct FlecsiRegisterTaskSimple : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterTaskSimple, flecsi_register_task_simple)
   std::string task;
   std::string processor;
   std::string launch;
};

template<>
struct MappingTraits<FlecsiRegisterTaskSimple> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterTaskSimple &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterTaskSimple>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(processor);
      kitsune_map(launch);
   }
};



// flecsi_register_task
struct FlecsiRegisterTask : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterTask, flecsi_register_task)
   std::string task;
   std::string nspace;
   std::string processor;
   std::string launch;
};

template<>
struct MappingTraits<FlecsiRegisterTask> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterTask &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterTask>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(nspace);
      kitsune_map(processor);
      kitsune_map(launch);
   }
};



// flecsi_register_mpi_task_simple
struct FlecsiRegisterMPITaskSimple : public flecsi_base {
   kitsune_macro_ctor(
      FlecsiRegisterMPITaskSimple,
      flecsi_register_mpi_task_simple)
   std::string task;
};

template<>
struct MappingTraits<FlecsiRegisterMPITaskSimple> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterMPITaskSimple &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterMPITaskSimple>");
      c.boilerplate(io);
      kitsune_map(task);
   }
};



// flecsi_register_mpi_task
struct FlecsiRegisterMPITask : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterMPITask, flecsi_register_mpi_task)
   std::string task;
   std::string nspace;
};

template<>
struct MappingTraits<FlecsiRegisterMPITask> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterMPITask &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterMPITask>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(nspace);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Task Execution
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_execute_task_simple     ( task,         launch, ... )
// flecsi_execute_task            ( task, nspace, launch, ... )
// flecsi_execute_mpi_task_simple ( task,                 ... )
// flecsi_execute_mpi_task        ( task, nspace,         ... )



// Helper: FlecsiVarArgType
// type only
struct FlecsiVarArgType {
   std::string type;
   inline FlecsiVarArgType() : type("") { }
   inline FlecsiVarArgType(
      const std::string &_type
   ) :
      type(_type)
   { }
};

template<>
struct MappingTraits<FlecsiVarArgType> {
   static void mapping(llvm::yaml::IO &io, FlecsiVarArgType &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiVarArgType>");
      kitsune_map(type);
   }
};



// Helper: FlecsiVarArgTypeValue
// type and value
struct FlecsiVarArgTypeValue {
   std::string type;
   std::string value;
   inline FlecsiVarArgTypeValue() : type(""), value("") { }
   inline FlecsiVarArgTypeValue(
      const std::string &_type,
      const std::string &_value
   ) :
      type (_type),
      value(_value)
   { }
};

template<>
struct MappingTraits<FlecsiVarArgTypeValue> {
   static void mapping(llvm::yaml::IO &io, FlecsiVarArgTypeValue &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiVarArgTypeValue>");
      kitsune_map(type);
      kitsune_map(value);
   }
};



// flecsi_execute_task_simple
struct FlecsiExecuteTaskSimple : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteTaskSimple, flecsi_execute_task_simple)
   std::string task;
   std::string launch;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiExecuteTaskSimple> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteTaskSimple &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteTaskSimple>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(launch);
      kitsune_map(varargs);
   }
};



// flecsi_execute_task
struct FlecsiExecuteTask : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteTask, flecsi_execute_task)
   std::string task;
   std::string nspace;
   std::string launch;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiExecuteTask> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteTask &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteTask>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(nspace);
      kitsune_map(launch);
      kitsune_map(varargs);
   }
};



// flecsi_execute_mpi_task_simple
struct FlecsiExecuteMPITaskSimple : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteMPITaskSimple, flecsi_execute_mpi_task_simple)
   std::string task;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiExecuteMPITaskSimple> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteMPITaskSimple &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteMPITaskSimple>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(varargs);
   }
};



// flecsi_execute_mpi_task
struct FlecsiExecuteMPITask : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteMPITask, flecsi_execute_mpi_task)
   std::string task;
   std::string nspace;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiExecuteMPITask> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteMPITask &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteMPITask>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(nspace);
      kitsune_map(varargs);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// *_global_object
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_register_global_object   ( index, nspace, type      )
// flecsi_set_global_object        ( index, nspace, type, obj )
// flecsi_initialize_global_object ( index, nspace, type, ... )
// flecsi_get_global_object        ( index, nspace, type      )



// flecsi_register_global_object
struct FlecsiRegisterGlobalObject : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterGlobalObject, flecsi_register_global_object)

   std::string index;
   std::string nspace;
   std::string type;
};

template<>
struct MappingTraits<FlecsiRegisterGlobalObject> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterGlobalObject &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterGlobalObject>");
      c.boilerplate(io);

      kitsune_map(index);
      kitsune_map(nspace);
      kitsune_map(type);
   }
};



// flecsi_set_global_object
struct FlecsiSetGlobalObject : public flecsi_base {
   kitsune_macro_ctor(FlecsiSetGlobalObject, flecsi_set_global_object)

   std::string index;
   std::string nspace;
   std::string type;
   std::string obj;
};

template<>
struct MappingTraits<FlecsiSetGlobalObject> {
   static void mapping(llvm::yaml::IO &io, FlecsiSetGlobalObject &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiSetGlobalObject>");
      c.boilerplate(io);

      kitsune_map(index);
      kitsune_map(nspace);
      kitsune_map(type);
      kitsune_map(obj);
   }
};



// flecsi_initialize_global_object
struct FlecsiInitializeGlobalObject : public flecsi_base {
   kitsune_macro_ctor(FlecsiInitializeGlobalObject, flecsi_initialize_global_object)

   std::string index;
   std::string nspace;
   std::string type;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiInitializeGlobalObject> {
   static void mapping(llvm::yaml::IO &io, FlecsiInitializeGlobalObject &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiInitializeGlobalObject>");
      c.boilerplate(io);

      kitsune_map(index);
      kitsune_map(nspace);
      kitsune_map(type);
      kitsune_map(varargs);
   }
};



// flecsi_get_global_object
struct FlecsiGetGlobalObject : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetGlobalObject, flecsi_get_global_object)

   std::string index;
   std::string nspace;
   std::string type;
};

template<>
struct MappingTraits<FlecsiGetGlobalObject> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetGlobalObject &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetGlobalObject>");
      c.boilerplate(io);

      kitsune_map(index);
      kitsune_map(nspace);
      kitsune_map(type);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// From FleCSI's execution.h
// Function Interface
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_register_function    ( func, nspace                   )
// flecsi_execute_function     (               handle,      ... )
// flecsi_function_handle      ( func, nspace                   )
// flecsi_define_function_type ( func,         return_type, ... )



// flecsi_register_function
struct FlecsiRegisterFunction : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterFunction, flecsi_register_function)
   std::string func;
   std::string nspace;
};

template<>
struct MappingTraits<FlecsiRegisterFunction> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterFunction &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterFunction>");
      c.boilerplate(io);
      kitsune_map(func);
      kitsune_map(nspace);
   }
};



// flecsi_execute_function
struct FlecsiExecuteFunction : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteFunction, flecsi_execute_function)
   std::string handle;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiExecuteFunction> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteFunction &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteFunction>");
      c.boilerplate(io);
      kitsune_map(handle);
      kitsune_map(varargs);
   }
};



// flecsi_function_handle
struct FlecsiFunctionHandle : public flecsi_base {
   kitsune_macro_ctor(FlecsiFunctionHandle, flecsi_function_handle)
   std::string func;
   std::string nspace;
};

template<>
struct MappingTraits<FlecsiFunctionHandle> {
   static void mapping(llvm::yaml::IO &io, FlecsiFunctionHandle &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiFunctionHandle>");
      c.boilerplate(io);
      kitsune_map(func);
      kitsune_map(nspace);
   }
};



// flecsi_define_function_type
struct FlecsiDefineFunctionType : public flecsi_base {
   kitsune_macro_ctor(FlecsiDefineFunctionType, flecsi_define_function_type)
   std::string func;
   std::string return_type;
   std::vector<FlecsiVarArgType> varargs;
};

template<>
struct MappingTraits<FlecsiDefineFunctionType> {
   static void mapping(llvm::yaml::IO &io, FlecsiDefineFunctionType &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiDefineFunctionType>");
      c.boilerplate(io);
      kitsune_map(func);
      kitsune_map(return_type);
      kitsune_map(varargs);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Registration
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_register_data_client
//    ( client_type, nspace, name )
// flecsi_register_field
//    ( client_type, nspace, name, data_type, storage_class, versions, ...)
// flecsi_register_global
//    (              nspace, name, data_type,                versions, ...)
// flecsi_register_color
//    (              nspace, name, data_type,                versions, ...)



// flecsi_register_data_client
struct FlecsiRegisterDataClient : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterDataClient, flecsi_register_data_client)
   std::string client_type;
   std::string nspace;
   std::string name;
};

template<>
struct MappingTraits<FlecsiRegisterDataClient> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterDataClient &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterDataClient>");
      c.boilerplate(io);
      kitsune_map(client_type);
      kitsune_map(nspace);
      kitsune_map(name);
   }
};



// flecsi_register_field
struct FlecsiRegisterField : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterField, flecsi_register_field)
   std::string   client_type;
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::string   storage_class;
   std::uint32_t versions;
   std::uint32_t index_space;
};

template<>
struct MappingTraits<FlecsiRegisterField> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterField &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterField>");
      c.boilerplate(io);
      kitsune_map(client_type);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(storage_class);
      kitsune_map(versions);
      kitsune_map(index_space);
   }
};



// flecsi_register_global
struct FlecsiRegisterGlobal : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterGlobal, flecsi_register_global)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t versions;
};

template<>
struct MappingTraits<FlecsiRegisterGlobal> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterGlobal &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterGlobal>");
      c.boilerplate(io);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(versions);
   }
};



// flecsi_register_color
struct FlecsiRegisterColor : public flecsi_base {
   kitsune_macro_ctor(FlecsiRegisterColor, flecsi_register_color)
   std::string   nspace;
   std::string   name;
   std::string   data_type;
   std::uint32_t versions;
};

template<>
struct MappingTraits<FlecsiRegisterColor> {
   static void mapping(llvm::yaml::IO &io, FlecsiRegisterColor &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiRegisterColor>");
      c.boilerplate(io);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(versions);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Some handle-related macros.
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_get_handle
//    ( client_handle, nspace, name, data_type, storage_class, version )
// flecsi_get_client_handle
//    ( client_type, nspace, name )
// flecsi_get_handles
//    ( client, nspace, data_type, storage_class, version, ... )
// flecsi_get_handles_all
//    ( client, data_type, storage_class, version, ... )



// flecsi_get_handle
struct FlecsiGetHandle : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetHandle, flecsi_get_handle)
   std::string client_handle;
   std::string nspace;
   std::string name;
   std::string data_type;
   std::string storage_class;
   std::uint32_t version;
};

template<>
struct MappingTraits<FlecsiGetHandle> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetHandle &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetHandle>");
      c.boilerplate(io);
      kitsune_map(client_handle);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(storage_class);
      kitsune_map(version);
   }
};



// flecsi_get_client_handle
struct FlecsiGetClientHandle : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetClientHandle, flecsi_get_client_handle)
   std::string client_type;
   std::string nspace;
   std::string name;
};

template<>
struct MappingTraits<FlecsiGetClientHandle> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetClientHandle &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetClientHandle>");
      c.boilerplate(io);
      kitsune_map(client_type);
      kitsune_map(nspace);
      kitsune_map(name);
   }
};



// flecsi_get_handles
struct FlecsiGetHandles : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetHandles, flecsi_get_handles)
   std::string client;
   std::string nspace;
   std::string data_type;
   std::string storage_class;
   std::string version;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiGetHandles> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetHandles &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetHandles>");
      c.boilerplate(io);
      kitsune_map(client);
      kitsune_map(nspace);
      kitsune_map(data_type);
      kitsune_map(storage_class);
      kitsune_map(version);
      kitsune_map(varargs);
   }
};



// flecsi_get_handles_all
struct FlecsiGetHandlesAll : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetHandlesAll, flecsi_get_handles_all)
   std::string client;
   std::string data_type;
   std::string storage_class;
   std::string version;
   std::vector<FlecsiVarArgTypeValue> varargs;
};

template<>
struct MappingTraits<FlecsiGetHandlesAll> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetHandlesAll &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetHandlesAll>");
      c.boilerplate(io);
      kitsune_map(client);
      kitsune_map(data_type);
      kitsune_map(storage_class);
      kitsune_map(version);
      kitsune_map(varargs);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// From FleCSI's data.h
// Some flecsi_get_* macros.
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

// FleCSI macros:
//
// flecsi_get_mutator
//    (client_handle, nspace, name, data_type, storage_class, version, slots)
// flecsi_get_global
//    (nspace, name, data_type, version)
// flecsi_get_color
//    (nspace, name, data_type, version)



// flecsi_get_mutator
struct FlecsiGetMutator : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetMutator, flecsi_get_mutator)
   std::string client_handle;
   std::string nspace;
   std::string name;
   std::string data_type;
   std::string storage_class;
   std::uint32_t version;
   std::string slots;
};

template<>
struct MappingTraits<FlecsiGetMutator> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetMutator &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetMutator>");
      c.boilerplate(io);
      kitsune_map(client_handle);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(storage_class);
      kitsune_map(version);
      kitsune_map(slots);
   }
};



// flecsi_get_global
struct FlecsiGetGlobal : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetGlobal, flecsi_get_global)
   std::string nspace;
   std::string name;
   std::string data_type;
   std::uint32_t version;
};

template<>
struct MappingTraits<FlecsiGetGlobal> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetGlobal &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetGlobal>");
      c.boilerplate(io);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(version);
   }
};



// flecsi_get_color
struct FlecsiGetColor : public flecsi_base {
   kitsune_macro_ctor(FlecsiGetColor, flecsi_get_color)
   std::string nspace;
   std::string name;
   std::string data_type;
   std::uint32_t version;
};

template<>
struct MappingTraits<FlecsiGetColor> {
   static void mapping(llvm::yaml::IO &io, FlecsiGetColor &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiGetColor>");
      c.boilerplate(io);
      kitsune_map(nspace);
      kitsune_map(name);
      kitsune_map(data_type);
      kitsune_map(version);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// FleCSIMetadata
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

struct FleCSIMetadata {
   #define kitsune_make_vector(name) std::vector<yaml::name> name

   kitsune_make_vector(FlecsiRegisterTaskSimple);
   kitsune_make_vector(FlecsiRegisterTask);
   kitsune_make_vector(FlecsiRegisterMPITaskSimple);
   kitsune_make_vector(FlecsiRegisterMPITask);

   kitsune_make_vector(FlecsiExecuteTaskSimple);
   kitsune_make_vector(FlecsiExecuteTask);
   kitsune_make_vector(FlecsiExecuteMPITaskSimple);
   kitsune_make_vector(FlecsiExecuteMPITask);

   kitsune_make_vector(FlecsiRegisterGlobalObject);
   kitsune_make_vector(FlecsiSetGlobalObject);
   kitsune_make_vector(FlecsiInitializeGlobalObject);
   kitsune_make_vector(FlecsiGetGlobalObject);

   kitsune_make_vector(FlecsiRegisterDataClient);
   kitsune_make_vector(FlecsiRegisterField);
   kitsune_make_vector(FlecsiRegisterGlobal);
   kitsune_make_vector(FlecsiRegisterColor);

   kitsune_make_vector(FlecsiRegisterFunction);
   kitsune_make_vector(FlecsiExecuteFunction);
   kitsune_make_vector(FlecsiFunctionHandle);
   kitsune_make_vector(FlecsiDefineFunctionType);

   kitsune_make_vector(FlecsiGetHandle);
   kitsune_make_vector(FlecsiGetClientHandle);
   kitsune_make_vector(FlecsiGetHandles);
   kitsune_make_vector(FlecsiGetHandlesAll);

   kitsune_make_vector(FlecsiGetMutator);
   kitsune_make_vector(FlecsiGetGlobal);
   kitsune_make_vector(FlecsiGetColor);

   #undef kitsune_make_vector
};

template<>
struct MappingTraits<FleCSIMetadata> {
   static void mapping(llvm::yaml::IO &io, FleCSIMetadata &c)
   {
      kitsune_debug("mapping(): MappingTraits<FleCSIMetadata>");

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
      kitsune_map(FlecsiGetHandles);
      kitsune_map(FlecsiGetHandlesAll);

      kitsune_map(FlecsiGetMutator);
      kitsune_map(FlecsiGetGlobal);
      kitsune_map(FlecsiGetColor);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// Helper functions
//    getVarArgsFromFunctionParameters
//    getVarArgsFromFunctionHandle
// -----------------------------------------------------------------------------

// getVarArgsFromFunctionParameters
// Function parameters: have type and value, so use FlecsiVarArgTypeValue.
void getVarArgsFromFunctionParameters(
   clang::CallExpr *const call,
   std::vector<llvm::yaml::FlecsiVarArgTypeValue> &varargs,
   const unsigned start = 0
) {
   clang::LangOptions LangOpts;
   LangOpts.CPlusPlus = true;
   clang::PrintingPolicy Policy(LangOpts);

   const unsigned narg = call->getNumArgs();
   for (unsigned arg = start;  arg < narg;  ++arg) {
      const clang::Expr *const expr = call->getArg(arg);

      if (expr == nullptr) {
         const std::string type  = "unknown type";
         const std::string value = "unknown value";
         varargs.push_back(llvm::yaml::FlecsiVarArgTypeValue(type,value));
      } else {
         // type
         const std::string type = expr->getType().getAsString();

         // value
         std::string s;
         llvm::raw_string_ostream rso(s);
         expr->printPretty(rso, 0, Policy);
         const std::string value = rso.str();

         varargs.push_back(llvm::yaml::FlecsiVarArgTypeValue(type,value));
      }
   }
}



/*
qqq working here; rework function

OLD:
   flecsi_define_function_type(func, return_type, ...)
      using func = flecsi::execution::function_handle__<
         return_type, std::tuple<__VA_ARGS__>
      >

NEW:

   flecsi_register_field(client_type, nspace, name, data_type,
                         storage_class, versions, ...)
      bool var = flecsi::data::field_interface_t::register_field<
         zero,
         one,
         two,
         three,
         four,
         five,
         ##__VA_ARGS__
      >(blah)

Relevant macros:
   (x) flecsi_register_field
   (n) flecsi_register_global
   (n) flecsi_register_color

// getVarArgsFromTemplateParameters
// Template parameters: have type only, so use FlecsiVarArgType.
void getVarArgsFromTemplateParameters(
   const clang::TemplateArgumentList &ta,
   std::vector<llvm::yaml::FlecsiVarArgType> &varargs,
   const unsigned pos = 0
) {
   // FYI
   kitsune_print(ta.size());  // 7
   kitsune_print(ta.asArray().size()); // 7

   if (pos < ta.size()) {
      kitsune_debug("parameter there: yes");
      const clang::TemplateArgument &arg = ta.get(pos);
      kitsune_print(arg.getKind());  // qqq? Getting "4" in both cases
      kitsune_print(arg.getKind() == clang::TemplateArgument::Integral); // 1
      // qqq extract as size_t
   } else {
      kitsune_debug("parameter there: no");
   }

///   kitsune_print(pack.pack_size());

///   // Let's see if we can successfully iterate over a pack
///   for (auto iter = pack.pack_begin();  iter != pack.pack_end();  ++iter) {
///      const clang::TemplateArgument &arg = *iter;
///      const std::string type = arg.getAsType().getAsString();
///      kitsune_print(type);
///      varargs.push_back(llvm::yaml::FlecsiVarArgType(type));
///   }
}
*/



// getVarArgsFromFunctionHandle
// Template parameters: have type only, so use FlecsiVarArgType.
void getVarArgsFromFunctionHandle(
   const clang::TypeAliasDecl *const alias,
   std::vector<llvm::yaml::FlecsiVarArgType> &varargs
) {
   // qqq clean up this function

   // Consider a type alias like this:
   //    using foo = bar<int,tuple<float,double>>
   // where bar is a regular class template with two template arguments,
   // and tuple is std::tuple - with a variadic template argument pack.

   // Right-hand side of the type alias:
   //    bar<int,tuple<float,double>>
   // in our hypothetic example.
   const clang::ClassTemplateSpecializationDecl *const rhs =
      clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
         alias->getUnderlyingType().getTypePtr()->getAsCXXRecordDecl()
      );

   // Template arguments of the right-hand side:
   //    int
   //    tuple<float,double>
   // in our hypothetic example.
   const clang::TemplateArgumentList &outer = rhs->getTemplateArgs();

   // FYI
   kitsune_print(outer.size());
   kitsune_print(outer.get(0).getAsType().getAsString());
   kitsune_print(outer.get(1).getAsType().getAsString());

   // Now, let's dig into the tuple. It turns out that getting its arguments
   // involves a different process than what we used above, due to its being
   // a variadic template class.

   // First, get the tuple itself, as a QualType
   clang::QualType qttup = outer.get(1).getAsType();

   // FYI
   ///qttup.getTypePtr()->dump();
   ///std::cerr << std::endl;

   // Next, get the tuple as a ClassTemplateSpecializationDecl
   const clang::ClassTemplateSpecializationDecl *const tuple =
      clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
         qttup.getTypePtr()->getAsCXXRecordDecl()
      );

   // FYI
   kitsune_print(tuple);

   // Template argument of the tuple. Because tuple is a variadic template
   // class, getTemplateArgs() in fact gives a "TemplateArgumentList" with
   // just one argument, which itself represents the argument pack.
   const clang::TemplateArgumentList &inner = tuple->getTemplateArgs();

   // FYI
   kitsune_print(inner.size()); // 1 (the pack)
   kitsune_print(inner.asArray().size()); // 1
   ///inner.get(0).dump();
   ///std::cerr << std::endl;

   // The following should be a pointer to a const TemplateArgument
   (void)&inner.get(0);

   const clang::TemplateArgument &pack = inner.get(0);
   kitsune_print(pack.getKind());  // tells me this IS A PACK

   // Confusingly, even though it's a pack (according to the above diagnotic),
   // both of these return 0 (false).
   kitsune_print(pack.containsUnexpandedParameterPack());
   kitsune_print(pack.isPackExpansion());
   kitsune_print(pack.pack_size());

   // Let's see if we can successfully iterate over a pack
   for (auto iter = pack.pack_begin();  iter != pack.pack_end();  ++iter) {
      const clang::TemplateArgument &arg = *iter;
      const std::string type = arg.getAsType().getAsString();
      kitsune_print(type);
      varargs.push_back(llvm::yaml::FlecsiVarArgType(type));
   }
}



// -----------------------------------------------------------------------------
// FleCSIAnalyzer::PreprocessorAnalyzer
// -----------------------------------------------------------------------------

namespace clang {
namespace sema {

class FleCSIAnalyzer::PreprocessorAnalyzer : public clang::PPCallbacks
{
   // The FleCSI macros that we'll analyze
   const std::set<std::string> flecsiMacros_;

   // Miscellaneous information (from clang) that we'll need here and there
   clang::Sema &sema_;
   clang::SourceManager &sourceMgr_;
   const clang::LangOptions &langOpts_;

   // FleCSI macro information
   std::map<
      clang::FileID,  // for some file...
      std::map<
         std::size_t, // at some location...
         MacroData    // we'll save information about a Flecsi-related macro
      >
   > sourceMap_;

   // It seems that this gets used via Analyzer::md_, which is a reference
   // that initializes to metadata() (i.e., this).
   llvm::yaml::FleCSIMetadata metadata_;

public:

   // constructor, destructor
   PreprocessorAnalyzer(clang::Sema &);
  ~PreprocessorAnalyzer();

   // PPCallbacks overrides
   void MacroDefined(
      const clang::Token &,
      const clang::MacroDirective *
   ) override;

   void MacroExpands(
      const clang::Token &,
      const clang::MacroDefinition &,
            clang::SourceRange,
      const clang::MacroArgs *
   ) override;

   // accessors
   MacroData *getMacroData(const clang::SourceLocation);
   llvm::yaml::FleCSIMetadata &metadata();
}; // class PreprocessorAnalyzer



// ------------------------
// Constructor
// Destructor
// ------------------------

FleCSIAnalyzer::PreprocessorAnalyzer::PreprocessorAnalyzer(clang::Sema &sema) :

   flecsiMacros_({
      "flecsi_register_task_simple",
      "flecsi_register_task",
      "flecsi_register_mpi_task_simple",
      "flecsi_register_mpi_task",

      "flecsi_execute_task_simple",
      "flecsi_execute_task",
      "flecsi_execute_mpi_task_simple",
      "flecsi_execute_mpi_task",

      "flecsi_register_global_object",
      "flecsi_set_global_object",
      "flecsi_initialize_global_object",
      "flecsi_get_global_object",

      "flecsi_register_data_client",
      "flecsi_register_field",
      "flecsi_register_global",
      "flecsi_register_color",

      "flecsi_register_function",
      "flecsi_execute_function",
      "flecsi_function_handle",
      "flecsi_define_function_type",

      "flecsi_get_handle",
      "flecsi_get_client_handle",
      "flecsi_get_handles",
      "flecsi_get_handles_all",

      "flecsi_get_mutator",
      "flecsi_get_global",
      "flecsi_get_color"
   }),

   sema_(sema),
   sourceMgr_(sema_.getSourceManager()),
   langOpts_(sema_.getLangOpts())
{
   kitsune_debug("PreprocessorAnalyzer::PreprocessorAnalyzer()");
}


inline FleCSIAnalyzer::PreprocessorAnalyzer::~PreprocessorAnalyzer()
{
   kitsune_debug("PreprocessorAnalyzer::~PreprocessorAnalyzer()");
}



// ------------------------
// MacroDefined
// MacroExpands
//
// These override the ones
// in the base PPCallbacks
// ------------------------

// MacroDefined
inline void FleCSIAnalyzer::PreprocessorAnalyzer::MacroDefined(
   const clang::Token &tok,
   const clang::MacroDirective *
) {
   kitsune_debug("PreprocessorAnalyzer::MacroDefined()");
   kitsune_debug(std::string("   tok.getName() == \"") + tok.getName() + '"');
}



// MacroExpands
inline void FleCSIAnalyzer::PreprocessorAnalyzer::MacroExpands(
   const clang::Token &mactok,
   const clang::MacroDefinition &def,
         clang::SourceRange range,
   const clang::MacroArgs *args
) {
   kitsune_debug("PreprocessorAnalyzer::MacroExpands()");

   // Macro's identifier info
   const clang::IdentifierInfo *const ii = mactok.getIdentifierInfo();
   if (ii == nullptr)
      return;

   // Macro's name
   const std::string macname = ii->getName().str();
   kitsune_print(macname);

   // If name isn't one of ours, we're done here
   if (flecsiMacros_.find(macname) == flecsiMacros_.end())
      return;

   // OK, we've recognized one of our macros.
   // Build a MacroData object for this particular macro call.
   MacroData macdata;
   macdata.tok = mactok;
   macdata.end = sourceMgr_.getDecomposedExpansionLoc(range.getEnd()).second;

   // Number of arguments to the macro
   const std::size_t narg = args->getNumMacroArguments();
   kitsune_print(narg);

   // For each macro argument
   for (std::size_t a = 0;  a < narg;  ++a) {
      // Pointer to first token of the argument (other tokens will follow)
      const clang::Token *const tokbegin = args->getUnexpArgument(a);

      macdata.args.push_back(std::vector<clang::Token>{});

      // For each token of the current macro argument
      for (const clang::Token *t = tokbegin;  t->isNot(tok::eof);  ++t)
         macdata.args[a].push_back(*t);
   }

   // Enter the new MacroData object into our map structure,
   // essentially a map(FileID,map(Offset,MacroData)).
   const std::pair<clang::FileID, unsigned> &pos =
      sourceMgr_.getDecomposedExpansionLoc(mactok.getLocation());
   FileID   fileid = pos.first;
   unsigned offset = pos.second;
   sourceMap_[fileid].insert(std::make_pair(offset,macdata));
}



// ------------------------
// getMacroData
// metadata
// ------------------------

// getMacroData
inline MacroData *FleCSIAnalyzer::PreprocessorAnalyzer::getMacroData(
   const clang::SourceLocation loc
) {
   kitsune_debug("PreprocessorAnalyzer::getMacroData()");

   // For the construct (declaration, expression, etc.) we're examining, get
   // its File ID and offset. We'll then see if it's associated with a macro.
   std::pair<FileID,unsigned> pos = sourceMgr_.getDecomposedExpansionLoc(loc);
   FileID   fileid = pos.first;
   unsigned offset = pos.second;

   // Look for File ID in our map of macro information
   auto itr = sourceMap_.find(fileid);
   if (itr == sourceMap_.end()) { // find failed
      kitsune_debug("getMacroData(): failure 1");
      return nullptr;
   }

   // The submap (offset --> MacroData) for the File ID we just found.
   // The keys of this submap are offsets of the starts of macro invocations.
   std::map<std::size_t, MacroData> &sub = itr->second;

   // Look for the largest submap entry that's <= offset. That macro would
   // be responsible for the present construct, if *any* macro is.
   auto mitr = greatest_less_or_equal(sub,offset);
   if (mitr == sub.end()) { // find failed; offset must be before any macro
      kitsune_debug("getMacroData(): failure 2");
      return nullptr;
   }

   // MacroData for the desired (FileID,offset)
   const MacroData  &macdata = mitr->second;
   const std::size_t begin   = mitr->first;
   const std::size_t end     = macdata.end;

   if (begin <= offset && offset <= end)
      return &mitr->second;

   kitsune_debug("getMacroData(): failure 3");
   return nullptr;
}


// metadata
inline llvm::yaml::FleCSIMetadata &
FleCSIAnalyzer::PreprocessorAnalyzer::metadata()
{
   kitsune_debug("PreprocessorAnalyzer::metadata()");
   return metadata_;
}

} // namespace sema
} // namespace clang



// -----------------------------------------------------------------------------
// Analyzer
// Unnamed namespace ("static"); so constructs are used only in this file.
// -----------------------------------------------------------------------------

namespace {

class Analyzer : public clang::RecursiveASTVisitor<Analyzer>
{
   clang::Sema &sema_;
   clang::sema::FleCSIAnalyzer::PreprocessorAnalyzer *pa_;
   llvm::yaml::FleCSIMetadata &md_;  // FleCSIMetadata: see above

public:
   Analyzer(
      clang::Sema &,
      clang::sema::FleCSIAnalyzer::PreprocessorAnalyzer *const
   );
  ~Analyzer();

   // qqq Can constness be made more consistent?...
   bool VisitVarDecl            (const clang::VarDecl *const);
   bool VisitCallExpr           (clang::CallExpr *const);
   bool VisitTypeAliasDecl      (clang::TypeAliasDecl *const);
   bool VisitTranslationUnitDecl(clang::TranslationUnitDecl *const);

   // qqq Can some/all of these (ALL remaining Analyzer member functions!)
   // be pulled outside of the class?
   std::string getName         (const clang::NamedDecl *const);
   std::string getQualifiedName(const clang::NamedDecl *const);

   /*
   const clang::TemplateArgumentList
     *getTemplateArgs(const clang::FunctionDecl *const);
   const clang::CXXRecordDecl
     *getClassDecl   (clang::QualType);
   const clang::CXXMethodDecl
     *getMethod      (const clang::CallExpr *const);

   std::int64_t
   getIntArg (const clang::TemplateArgumentList *const, const std::size_t);
   std::uint64_t
   getUIntArg(const clang::TemplateArgumentList *const, const std::size_t);
   clang::QualType
   getTypeArg(const clang::TemplateArgumentList *const, const std::size_t);
   */

   const clang::CallExpr *getClassCall( // 3-argument
      const clang::Expr *const, const std::string &, const std::string &);
   const clang::CallExpr *getClassCall( // 4-argument
      const clang::Expr *const, const std::string &, const std::string &,
      const int);
   const clang::CallExpr *getClassCall( // 5-argument
      const clang::Expr *const, const std::string &, const std::string &,
      const int, const clang::CXXRecordDecl * &);
   const clang::CallExpr *getClassCall( // 6-argument
      const clang::Expr *,      const std::string &, const std::string &,
      const int, const int, const clang::CXXRecordDecl * &);

   const clang::Expr *normExpr(const clang::Expr *const);
   bool isDerivedFrom(const clang::CXXRecordDecl *const, const std::string &);
};



// ------------------------
// Constructor
// Destructor
// ------------------------

// This is only called from within FleCSIAnalyzer::gatherMetadata()
Analyzer::Analyzer(
   clang::Sema &sema,
   clang::sema::FleCSIAnalyzer::PreprocessorAnalyzer *const pa
)
 : sema_(sema),
   pa_(pa),
   md_(pa_->metadata())
{
   kitsune_debug("Analyzer::Analyzer()");
}

Analyzer::~Analyzer()
{
   kitsune_debug("Analyzer::~Analyzer()");
}



// -----------------------------------------------------------------------------
// VisitVarDecl
// -----------------------------------------------------------------------------

bool Analyzer::VisitVarDecl(const clang::VarDecl *const var)
{
   kitsune_debug("Analyzer::VisitVarDecl()");
   kitsune_print(var->getNameAsString());

   // *_registered
   if (!var->getName().endswith("_registered"))
      return true;

   // Associated MacroData
   const MacroData *const macptr = pa_->getMacroData(var->getLocStart());
   if (!macptr)
      return true;
   const MacroData &macdata = *macptr;

   std::string name = tostr(macdata.tok,sema_);
   kitsune_print(name);
   const clang::CXXRecordDecl *rd;



   // flecsi_register_task_simple(task, processor, launch)
   if (name == "flecsi_register_task_simple") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__", "register_task",
         3, rd
      );

      if (call != nullptr) {
         llvm::yaml::FlecsiRegisterTaskSimple c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task      = macdata.str(pos++,sema_);
         c.processor = macdata.str(pos++,sema_);
         c.launch    = macdata.str(pos++,sema_);
         md_.FlecsiRegisterTaskSimple.push_back(c);
      }
   }



   // flecsi_register_task(task, nspace, processor, launch)
   if (name == "flecsi_register_task") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__", "register_task",
         3, rd
      );

      if (call != nullptr) {
         llvm::yaml::FlecsiRegisterTask c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task      = macdata.str(pos++,sema_);
         c.nspace    = macdata.str(pos++,sema_);
         c.processor = macdata.str(pos++,sema_);
         c.launch    = macdata.str(pos++,sema_);
         md_.FlecsiRegisterTask.push_back(c);
      }
   }



   // flecsi_register_mpi_task_simple(task)
   if (name == "flecsi_register_mpi_task_simple") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__", "register_task",
         3, rd
      );

      if (call != nullptr) {
         llvm::yaml::FlecsiRegisterMPITaskSimple c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task = macdata.str(pos++,sema_);
         md_.FlecsiRegisterMPITaskSimple.push_back(c);
      }
   }



   // flecsi_register_mpi_task(task, nspace)
   if (name == "flecsi_register_mpi_task") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__", "register_task",
         3, rd
      );

      if (call != nullptr) {
         llvm::yaml::FlecsiRegisterMPITask c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task   = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         md_.FlecsiRegisterMPITask.push_back(c);
      }
   }



   // flecsi_register_data_client(client_type, nspace, name)
   if (name == "flecsi_register_data_client") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::data_client_interface__", "register_data_client",
         1, rd
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         const clang::QualType qt = getTypeArg(ta,0);
         const clang::CXXRecordDecl *const cd = getClassDecl(qt);
         if (!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")) {
            sema_.Diag(
               macdata.loc(0,0),
               clang::diag::err_flecsi_not_a_data_client);
            return true;
         }

         llvm::yaml::FlecsiRegisterDataClient c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client_type = macdata.str(pos++,sema_);
         c.nspace      = macdata.str(pos++,sema_);
         c.name        = macdata.str(pos++,sema_);
         md_.FlecsiRegisterDataClient.push_back(c);
      }
   }



   // flecsi_register_field(client_type, nspace, name, data_type,
   //                       storage_class, versions, ...)
   if (name == "flecsi_register_field") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::field_interface__", "register_field",
         1, rd
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiRegisterField c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client_type   = macdata.str(pos++,sema_);
         c.nspace        = macdata.str(pos++,sema_);
         c.name          = macdata.str(pos++,sema_);
         c.data_type     = macdata.str(pos++,sema_);
         c.storage_class = macdata.str(pos++,sema_);
         c.versions    = getUIntArg(ta,5); // 5 = template argument position
         c.index_space = getUIntArg(ta,6);
         md_.FlecsiRegisterField.push_back(c);
      }
   }



   // flecsi_register_global(nspace, name, data_type, versions, ...)
   if (name == "flecsi_register_global") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::field_interface__", "register_field",
         1, rd
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiRegisterGlobal c(&sema_,&macdata);
         std::size_t pos = 0;
         c.nspace    = macdata.str(pos++,sema_);
         c.name      = macdata.str(pos++,sema_);
         c.data_type = macdata.str(pos++,sema_);
         c.versions  = getUIntArg(ta,5);
         // qqq For an index_space as before, the macro seems to set things
         // up so that we have ...::global_is followed by __VA_ARGS__. Think
         // about how to deal with this. Should we just, for instance, save
         // the variadic stuff, and consider global_is to be implied by the
         // fact that we're dealing with (YAML-izing) this macro?
         // c.indexSpace = getUIntArg(ta,6);
         ///getVarArgsFromTemplateParameters(*ta, c.varargs, 7);
         md_.FlecsiRegisterGlobal.push_back(c);
      }
   }



   // flecsi_register_color(nspace, name, data_type, versions, ...)
   if (name == "flecsi_register_color") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::field_interface__", "register_field",
         1, rd
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiRegisterColor c(&sema_,&macdata);
         std::size_t pos = 0;
         c.nspace    = macdata.str(pos++,sema_);
         c.name      = macdata.str(pos++,sema_);
         c.data_type = macdata.str(pos++,sema_);
         c.versions  = getUIntArg(ta,5);
         // c.indexSpace = getUIntArg(ta,6);
         ///getVarArgsFromTemplateParameters(*ta, c.varargs, 7);
         md_.FlecsiRegisterColor.push_back(c);
      }
   }



   // flecsi_register_function(func, nspace)
   if (name == "flecsi_register_function") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::function_interface__", "register_function",
         0, rd
      );

      if (call != nullptr) {
         llvm::yaml::FlecsiRegisterFunction c(&sema_,&macdata);
         std::size_t pos = 0;
         c.func   = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         md_.FlecsiRegisterFunction.push_back(c);
      }
   }



   return true;
}



// -----------------------------------------------------------------------------
// VisitCallExpr
// -----------------------------------------------------------------------------

bool Analyzer::VisitCallExpr(clang::CallExpr *const call)
{
   kitsune_debug("Analyzer::VisitCallExpr()");

   clang::Decl *cd = call->getCalleeDecl();
   if (!cd)
      return true;

   auto fd = clang::dyn_cast<clang::FunctionDecl>(cd);
   if (!fd)
      return true;

   clang::DeclarationName dn = fd->getDeclName();
   if (!dn || !dn.isIdentifier())
      return true;

   const MacroData *const macptr = pa_->getMacroData(call->getLocStart());
   if (!macptr)
      return true;
   const MacroData &macdata = *macptr;

   std::string name = tostr(macdata.tok,sema_);
   kitsune_print(name);
   const clang::CXXRecordDecl *rd;
   (void)rd;//qqq



   // flecsi_execute_task_simple(task, launch, ...)
   if (name == "flecsi_execute_task_simple") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__", "execute_task"
      )) {
         llvm::yaml::FlecsiExecuteTaskSimple c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task   = macdata.str(pos++,sema_);
         c.launch = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs);
         md_.FlecsiExecuteTaskSimple.push_back(c);
      }
   }

   // flecsi_execute_task(task, nspace, launch, ...)
   if (name == "flecsi_execute_task") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__", "execute_task"
      )) {
         llvm::yaml::FlecsiExecuteTask c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task   = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         c.launch = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs);
         md_.FlecsiExecuteTask.push_back(c);
      }
   }

   // flecsi_execute_mpi_task_simple(task, ...)
   if (name == "flecsi_execute_mpi_task_simple") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__", "execute_task"
      )) {
         llvm::yaml::FlecsiExecuteMPITaskSimple c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs);
         md_.FlecsiExecuteMPITaskSimple.push_back(c);
      }
   }

   // flecsi_execute_mpi_task(task, nspace, ...)
   if (name == "flecsi_execute_mpi_task") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__", "execute_task"
      )) {
         llvm::yaml::FlecsiExecuteMPITask c(&sema_,&macdata);
         std::size_t pos = 0;
         c.task   = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs);
         md_.FlecsiExecuteMPITask.push_back(c);
      }
   }



   // flecsi_register_global_object(index, nspace, type)
   if (name == "flecsi_register_global_object") {
      if (getClassCall(
         call,
        "flecsi::execution::context__", "register_global_object"
      )) {
         llvm::yaml::FlecsiRegisterGlobalObject c(&sema_,&macdata);
         std::size_t pos = 0;
         c.index  = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         c.type   = macdata.str(pos++,sema_);
         md_.FlecsiRegisterGlobalObject.push_back(c);
      }
   }

   // flecsi_set_global_object(index, nspace, type, obj)
   if (name == "flecsi_set_global_object") {
      if (getClassCall(
         call,
        "flecsi::execution::context__", "set_global_object"
      )) {
         llvm::yaml::FlecsiSetGlobalObject c(&sema_,&macdata);
         std::size_t pos = 0;
         c.index  = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         c.type   = macdata.str(pos++,sema_);
         c.obj    = macdata.str(pos++,sema_);
         md_.FlecsiSetGlobalObject.push_back(c);
      }
   }

   // flecsi_initialize_global_object(index, nspace, type, ...)
   if (name == "flecsi_initialize_global_object") {
      if (getClassCall(
         call,
        "flecsi::execution::context__", "initialize_global_object"
      )) {
         llvm::yaml::FlecsiInitializeGlobalObject c(&sema_,&macdata);
         std::size_t pos = 0;
         c.index  = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         c.type   = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs, 1);
         md_.FlecsiInitializeGlobalObject.push_back(c);
      }
   }

   // flecsi_get_global_object(index, nspace, type)
   if (name == "flecsi_get_global_object") {
      if (getClassCall(
         call,
        "flecsi::execution::context__", "get_global_object"
      )) {
         llvm::yaml::FlecsiGetGlobalObject c(&sema_,&macdata);
         std::size_t pos = 0;
         c.index  = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         c.type   = macdata.str(pos++,sema_);
         md_.FlecsiGetGlobalObject.push_back(c);
      }
   }

   // flecsi_execute_function(handle, ...)
   if (name == "flecsi_execute_function") {
      if (getClassCall(
         call,
        "flecsi::execution::function_interface__", "execute_function"
      )) {
         llvm::yaml::FlecsiExecuteFunction c(&sema_,&macdata);
         std::size_t pos = 0;
         c.handle = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs, 1);
         md_.FlecsiExecuteFunction.push_back(c);
      }
   }

   // flecsi_function_handle(func, nspace)
   if (name == "flecsi_function_handle") {
      kitsune_debug("almost found 2: function handle");

      if (getClassCall(
         call,
        "flecsi::utils::const_string_t", "hash"
      )) {
         llvm::yaml::FlecsiFunctionHandle c(&sema_,&macdata);
         std::size_t pos = 0;
         c.func   = macdata.str(pos++,sema_);
         c.nspace = macdata.str(pos++,sema_);
         md_.FlecsiFunctionHandle.push_back(c);
      }
   }



   // flecsi_get_handle
   if (name == "flecsi_get_handle") {
      if (getClassCall(
         call,
        "flecsi::data::field_interface__", "get_handle",
         1, rd
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiGetHandle c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client_handle = macdata.str(pos++,sema_);
         c.nspace        = macdata.str(pos++,sema_);
         c.name          = macdata.str(pos++,sema_);
         c.data_type     = macdata.str(pos++,sema_);
         c.storage_class = macdata.str(pos++,sema_);
         c.version       = getUIntArg(ta,5); // 5 = template argument position
         md_.FlecsiGetHandle.push_back(c);
      }
   }

   // flecsi_get_client_handle
   if (name == "flecsi_get_client_handle") {
      if (getClassCall(
         call,
        "flecsi::data::data_client_interface__", "get_client_handle"
      )) {
         llvm::yaml::FlecsiGetClientHandle c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client_type = macdata.str(pos++,sema_);
         c.nspace      = macdata.str(pos++,sema_);
         c.name        = macdata.str(pos++,sema_);
         md_.FlecsiGetClientHandle.push_back(c);
      }
   }



   // flecsi_get_mutator
   if (name == "flecsi_get_mutator") {
      if (getClassCall(
         call,
        "flecsi::data::field_interface__", "get_mutator",
         2, rd
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiGetMutator c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client_handle = macdata.str(pos++,sema_);
         c.nspace        = macdata.str(pos++,sema_);
         c.name          = macdata.str(pos++,sema_);
         c.data_type     = macdata.str(pos++,sema_);
         c.storage_class = macdata.str(pos++,sema_);
         c.version       = getUIntArg(ta,5); // 5 = template argument position
         pos++;  // version pulled via other means; skip to slots
         c.slots         = macdata.str(pos++,sema_);
         md_.FlecsiGetMutator.push_back(c);
      }
   }

   // flecsi_get_global
   if (name == "flecsi_get_global") {
      if (getClassCall( // this macro invokes the flecsi_get_handle macro...
         call,
        "flecsi::data::field_interface__", "get_handle",
         1, rd
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiGetGlobal c(&sema_,&macdata);
         std::size_t pos = 0;
         c.nspace    = macdata.str(pos++,sema_);
         c.name      = macdata.str(pos++,sema_);
         c.data_type = macdata.str(pos++,sema_);
         c.version   = getUIntArg(ta,5); // 5 = template argument position
         md_.FlecsiGetGlobal.push_back(c);
      }
   }

   // flecsi_get_color
   if (name == "flecsi_get_color") {
      if (getClassCall( // this macro invokes the flecsi_get_handle macro...
         call,
        "flecsi::data::field_interface__", "get_handle",
         1, rd
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiGetColor c(&sema_,&macdata);
         std::size_t pos = 0;
         c.nspace    = macdata.str(pos++,sema_);
         c.name      = macdata.str(pos++,sema_);
         c.data_type = macdata.str(pos++,sema_);
         c.version   = getUIntArg(ta,5); // 5 = template argument position
         md_.FlecsiGetColor.push_back(c);
      }
   }



   // qqq Still relevant, or deprecated...?
   // flecsi_get_handles
   if (name == "flecsi_get_handles") {
      if (getClassCall(
         call,
        "flecsi::data::field_interface__", "get_handles",
         2, std::numeric_limits<int>::max(), rd
      )) {
         llvm::yaml::FlecsiGetHandles c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client        = macdata.str(pos++,sema_);
         c.nspace        = macdata.str(pos++,sema_);
         c.data_type     = macdata.str(pos++,sema_);
         c.storage_class = macdata.str(pos++,sema_);
         c.version       = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs, 2);
         md_.FlecsiGetHandles.push_back(c);
      }
   }

   // qqq Still relevant, or deprecated...?
   // flecsi_get_handles_all
   if (name == "flecsi_get_handles_all") {
      if (getClassCall(
         call,
        "flecsi::data::field_interface__", "get_handles",
         2, std::numeric_limits<int>::max(), rd
      )) {
         llvm::yaml::FlecsiGetHandlesAll c(&sema_,&macdata);
         std::size_t pos = 0;
         c.client        = macdata.str(pos++,sema_);
         c.data_type     = macdata.str(pos++,sema_);
         c.storage_class = macdata.str(pos++,sema_);
         c.version       = macdata.str(pos++,sema_);
         getVarArgsFromFunctionParameters(call, c.varargs, 2);
         md_.FlecsiGetHandlesAll.push_back(c);
      }
   }



   /*
   qqq KEEP AROUND. HAS SOME CONSTRUCTS I DON'T SEE ELSEWHERE

   // flecsi_get_client_handle
   else if (name == "flecsi_get_client_handle") {
      if (getClassCall(
             call,
            "flecsi::data::client_data__", "get_client_handle", 0, rd
      )) {
         const clang::CXXMethodDecl *md = getMethod(call);
         const clang::TemplateArgumentList *templateArgs = getTemplateArgs(md);
         clang::QualType qt = getTypeArg(templateArgs,0);
         const clang::CXXRecordDecl *cd = getClassDecl(qt);

         if (!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")) {
            sema_.Diag(macdata.loc(0), clang::diag::err_flecsi_not_a_data_client);
            return true;
         }

         llvm::yaml::GetClientHandle c;
         c.line     = getFileLine(macdata, c.file);
         c.meshType = macdata.str(pos++,sema_);
         c.nspace   = macdata.str(pos++,sema_);
         c.name     = macdata.str(pos++,sema_);
         md_.clientHandleGets.push_back(c);
      }
   }
   */

   return true;
}



// -----------------------------------------------------------------------------
// VisitTypeAliasDecl
// -----------------------------------------------------------------------------

bool Analyzer::VisitTypeAliasDecl(clang::TypeAliasDecl *const ta)
{
   kitsune_debug("Analyzer::VisitTypeAliasDecl()");
   kitsune_print(ta->getNameAsString());

   // Associated MacroData
   const MacroData *const macptr = pa_->getMacroData(ta->getLocStart());
   if (!macptr)
      return true;
   const MacroData &macdata = *macptr;

   std::string name = tostr(macdata.tok,sema_);
   kitsune_print(name);

   // flecsi_define_function_type(func, return_type, ...)
   if (name == "flecsi_define_function_type") {
      llvm::yaml::FlecsiDefineFunctionType c(&sema_,&macdata);
      std::size_t pos = 0;
      c.func        = macdata.str(pos++,sema_);
      c.return_type = macdata.str(pos++,sema_);
      getVarArgsFromFunctionHandle(ta, c.varargs);
      md_.FlecsiDefineFunctionType.push_back(c);
   }

   return true;
}



// -----------------------------------------------------------------------------
// VisitTranslationUnitDecl
// -----------------------------------------------------------------------------

// qqq is this function ever called??
bool Analyzer::VisitTranslationUnitDecl(clang::TranslationUnitDecl *const d)
{
   kitsune_debug("Analyzer::VisitTranslationUnitDecl()");

   for (auto di : d->decls())
      TraverseDecl(di);
   return true;
}



// ------------------------
// getName
// getQualifiedName
// ------------------------

std::string Analyzer::getName(const clang::NamedDecl *const nd)
{
   kitsune_debug("Analyzer::getName()");

   clang::DeclarationName dn = nd->getDeclName();
   if (!dn || !dn.isIdentifier())
      return "";
   return dn.getAsString();
}



std::string Analyzer::getQualifiedName(const clang::NamedDecl *const nd)
{
   kitsune_debug("Analyzer::getQualifiedName()");

   return nd->getQualifiedNameAsString();
}



// ------------------------
// getClassCall
// ------------------------

// 3-argument: Expr, string, string
const clang::CallExpr *Analyzer::getClassCall(
   const clang::Expr *const e,
   const std::string &className,
   const std::string &call
) {
   kitsune_debug("Analyzer::getClassCall(), 3-argument");

   const clang::CXXRecordDecl *rd;
   return getClassCall( // 6-argument
      e, className, call,
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::max(),
      rd
   );
}



// 4-argument: Expr, string, string, int
const clang::CallExpr *Analyzer::getClassCall(
   const clang::Expr *const e,
   const std::string &className,
   const std::string &call,
   const int numArgs
) {
   kitsune_debug("Analyzer::getClassCall(), 4-argument");

   const clang::CXXRecordDecl *rd;
   return getClassCall( // 5-argument
      e, className, call,
      numArgs, rd
   );
}



// 5-argument: Expr, string, string, int, CXXRecordDecl
const clang::CallExpr *Analyzer::getClassCall(
   const clang::Expr *const e,
   const std::string &className,
   const std::string &call,
   const int numArgs,
   const clang::CXXRecordDecl * &rd
) {
   kitsune_debug("Analyzer::getClassCall(), 5-argument");

   return getClassCall( // 6-argument
      e, className, call,
      numArgs, numArgs, rd
   );
}



// 6-argument: Expr, string, string, int, int, CXXRecordDecl
const clang::CallExpr *Analyzer::getClassCall(
   const clang::Expr *e,
   const std::string &className,
   const std::string &call,
   const int minArgs,
   const int maxArgs,
   const clang::CXXRecordDecl * &rd
) {
   kitsune_debug("Analyzer::getClassCall(), 6-argument");

   if (!e)
      return nullptr;
   e = normExpr(e);

   auto callexpr = clang::dyn_cast<clang::CallExpr>(e);
   if (!callexpr)
      return nullptr;

   const clang::CXXMethodDecl *md = getMethod(callexpr);
   if (!md)
      return nullptr;

   rd = md->getParent();

   int numArgs = callexpr->getNumArgs();
   kitsune_print(numArgs);
   if (numArgs < minArgs || numArgs > maxArgs)
      return nullptr;

   kitsune_debug(std::string("string 1: ") + className);
   kitsune_debug(std::string("string 2: ") + getQualifiedName(rd));
   kitsune_debug(std::string("string 3: ") + call);
   kitsune_debug(std::string("string 4: ") + getName(md));

   return className == getQualifiedName(rd) && call == getName(md)
      ? callexpr
      : nullptr;
}



// ------------------------
// normExpr
// isDerivedFrom
// ------------------------

const clang::Expr *Analyzer::normExpr(const clang::Expr *const e)
{
   kitsune_debug("Analyzer::normExpr()");

   if (auto ec = clang::dyn_cast<clang::ExprWithCleanups>(e))
      return ec->getSubExpr();
   return e;
}


bool Analyzer::isDerivedFrom(
   const clang::CXXRecordDecl *const cd,
   const std::string &qualifiedBaseName
) {
   kitsune_debug("Analyzer::isDerivedFrom()");

   if (getQualifiedName(cd) == qualifiedBaseName)
      return true;

   for (auto bi : cd->bases()) {
      const clang::CXXRecordDecl *bcd =
         bi.getType().getTypePtr()->getAsCXXRecordDecl();
      if (bcd && isDerivedFrom(bcd, qualifiedBaseName))
         return true;
   }

   return false;
}

} // namespace



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Definitions
// For FleCSIAnalyzer member functions.
// See FleCSIAnalyzer.h
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

namespace clang {
namespace sema {

// instance
// static
FleCSIAnalyzer &FleCSIAnalyzer::instance(Sema *const sema)
{
   kitsune_debug("FleCSIAnalyzer::instance()");
   static FleCSIAnalyzer obj(*sema);
   return obj;
}



// constructor
FleCSIAnalyzer::FleCSIAnalyzer(Sema &sema)
 : sema_(sema),
   pa_(new PreprocessorAnalyzer(sema))
{
   kitsune_debug("FleCSIAnalyzer::FleCSIAnalyzer() begin");
   sema_.getPreprocessor().addPPCallbacks(
      std::unique_ptr<PreprocessorAnalyzer>(pa_));
   kitsune_debug("FleCSIAnalyzer::FleCSIAnalyzer() end");
}



// gatherMetadata
// Seems to be called many times
void FleCSIAnalyzer::gatherMetadata(Decl *decl)
{
   kitsune_debug("FleCSIAnalyzer::gatherMetadata()");

   Analyzer analyzer(sema_, pa_);
   kitsune_debug("call TraverseDecl()");
   analyzer.TraverseDecl(decl);
   kitsune_debug("done TraverseDecl()");
}



// finalizeMetadata
// Seems to be called just once
void FleCSIAnalyzer::finalizeMetadata(const CompilerInstance &)
{
   kitsune_debug("FleCSIAnalyzer::finalizeMetadata()");

   // get metadata
   llvm::yaml::FleCSIMetadata &meta = pa_->metadata();

   // write metadata to string
   std::string str;
   llvm::raw_string_ostream raw(str);
   llvm::yaml::Output yout(raw);
   yout << meta;

   raw.flush(); // <-- necessary
   kitsune_print(str);

   // open file
   // qqq Eventually, figure out where this goes (w/o the full path,
   // which we obviously don't really want).
   std::ofstream ofs("/home/staley/llvm/f/.flecsi-analyzer");
   ///std::ofstream ofs(".flecsi-analyzer");

   // write string to file
   ofs << raw.str();
}



// destructor
FleCSIAnalyzer::~FleCSIAnalyzer()
{
   kitsune_debug("FleCSIAnalyzer::~FleCSIAnalyzer()");
   delete pa_;
}

} // namespace sema
} // namespace clang
