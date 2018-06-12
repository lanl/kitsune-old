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
   #define kitsune_print(arg) \
   std::cout << "kitsune: " #arg " == " << (arg) << std::endl
#else
   #define kitsune_print(arg)
#endif



namespace {

// kitsune_debug
inline void kitsune_debug(const std::string &str, const bool newline = true)
{
   #ifdef KITSUNE_DEBUG
      std::cout << "kitsune: " << str;
      if (newline)
         std::cout << std::endl;
   #else
      (void)str;
      (void)newline;
   #endif
}

// print_type(name)
inline void print_type(const char *const name)
{
   #ifdef KITSUNE_DEBUG
      kitsune_debug(boost::core::demangle(name));
   #else
      (void)name;
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

} // namespace



// -----------------------------------------------------------------------------
// Helper constructs
//    str
//    MacroData
//    getFileLine
// Unnamed namespace ("static"); so constructs are used only in this file.
// -----------------------------------------------------------------------------

namespace {

// str
std::string str(const clang::Token &token)
{
   kitsune_debug("str()");
   return token.getIdentifierInfo()->getName().str();
}


// MacroData
//    arg()
//    loc()
struct MacroData
{
   clang::Token tok;
   // qqq At some point, this should probably be a vector<vector<Token>>...
   std::vector<clang::Token> args;

   std::string arg(const std::size_t i) const
   {
      kitsune_debug("MacroData::arg()");
      return str(args[i]);
   }

   clang::SourceLocation loc(const std::size_t i) const
   {
      kitsune_debug("MacroData::loc()");
      return args[i].getLocation();
   }
};


// getFileLine
// Gets both the file and the line number. The file is returned in the second
// parameter, while the line number is returned as the function's return value.
std::uint32_t getFileLine(
   const clang::Sema &sema_,
   const MacroData &macdata,
   std::string &file
) {
   kitsune_debug("getFileLine()");

   clang::SourceManager &srcMgr = sema_.getSourceManager();
   clang::SourceLocation loc = macdata.tok.getLocation();

   file = srcMgr.getFilename(loc).str();
   auto p = srcMgr.getDecomposedLoc(loc);
   return srcMgr.getLineNumber(p.first, p.second);
}

} // namespace



// -----------------------------------------------------------------------------
// flecsi_base
// Helper.
// Has type, file, line.
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

struct flecsi_base
{
   std::string   type;
   std::string   file;
   std::uint32_t line;

   flecsi_base(
      const std::string &_type,
      const clang::Sema *const sema = nullptr,
      const MacroData *const macdata = nullptr
   )
    : type(_type)
   {
      if (sema && macdata)
         line = getFileLine(*sema, *macdata, file);
   }

   void boilerplate(llvm::yaml::IO &io)
   {
      io.mapRequired("type", type);
      io.mapRequired("file", file);
      io.mapRequired("line", line);
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// SequenceTraits<vector<T>>
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

template<class T>
struct SequenceTraits<std::vector<T>>
{
   static std::size_t size(IO &, std::vector<T> &vec)
   {
      kitsune_debug("size()");
      return vec.size();
   }

   static T &element(IO &, std::vector<T> &vec, const std::size_t index)
   {
      kitsune_debug("element()");
      return vec[index];
   }
};

} // namespace yaml
} // namespace llvm



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

#define kitsune_macro_ctor(cname,mname)  /* class name, macro name */ \
   cname( \
      const clang::Sema *const sema = nullptr, \
      const MacroData   *const data = nullptr \
   ) : \
      flecsi_base(#mname, sema, data) \
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



// flecsi_execute_task_simple
struct FlecsiExecuteTaskSimple : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteTaskSimple, flecsi_execute_task_simple)
   std::string task;
   std::string launch;
};

template<>
struct MappingTraits<FlecsiExecuteTaskSimple> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteTaskSimple &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteTaskSimple>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(launch);
   }
};



// flecsi_execute_task
struct FlecsiExecuteTask : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteTask, flecsi_execute_task)
   std::string task;
   std::string nspace;
   std::string launch;
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
   }
};



// flecsi_execute_mpi_task_simple
struct FlecsiExecuteMPITaskSimple : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteMPITaskSimple, flecsi_execute_mpi_task_simple)
   std::string task;
};

template<>
struct MappingTraits<FlecsiExecuteMPITaskSimple> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteMPITaskSimple &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteMPITaskSimple>");
      c.boilerplate(io);
      kitsune_map(task);
   }
};



// flecsi_execute_mpi_task
struct FlecsiExecuteMPITask : public flecsi_base {
   kitsune_macro_ctor(FlecsiExecuteMPITask, flecsi_execute_mpi_task)
   std::string task;
   std::string nspace;
};

template<>
struct MappingTraits<FlecsiExecuteMPITask> {
   static void mapping(llvm::yaml::IO &io, FlecsiExecuteMPITask &c)
   {
      kitsune_debug("mapping(): MappingTraits<FlecsiExecuteMPITask>");
      c.boilerplate(io);
      kitsune_map(task);
      kitsune_map(nspace);
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
   ///std::uint32_t indexSpace;
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



/*
qqq

// -----------------------------------------------------------------------------
// GetHandle
// -----------------------------------------------------------------------------

struct GetHandle : public flecsi_base {
   GetHandle() : flecsi_base("get_handle") { }

   std::string nspace;
   std::string name;
};

template<>
struct MappingTraits<GetHandle> {
   static void mapping(llvm::yaml::IO &io, GetHandle &c)
   {
      kitsune_debug("mapping(): MappingTraits<GetHandle>");

      c.boilerplate(io);
      io.mapRequired("nspace", c.nspace);
      io.mapRequired("name",   c.name);
   }
};



// ------------------------
// GetClientHandle
// ------------------------

struct GetClientHandle : public flecsi_base {
   GetClientHandle() : flecsi_base("get_client_handle") { }
   std::uint32_t line;

   std::string   meshType;
   std::string   nspace;
   std::string   name;
};

template<>
struct MappingTraits<GetClientHandle> {
   static void mapping(llvm::yaml::IO &io, GetClientHandle &c)
   {
      kitsune_debug("mapping(): MappingTraits<GetClientHandle>");

      c.boilerplate(io);
      io.mapRequired("meshType", c.meshType);
      io.mapRequired("nspace",   c.nspace);
      io.mapRequired("name",     c.name);
   }
};
*/



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

   kitsune_make_vector(FlecsiRegisterDataClient);
   kitsune_make_vector(FlecsiRegisterField);
   kitsune_make_vector(FlecsiRegisterGlobal);
   kitsune_make_vector(FlecsiRegisterColor);

   /*
   // qqq
   std::vector< yaml::GetHandle>          handleGets;
   std::vector< yaml::GetClientHandle>    clientHandleGets;
   */

   #undef kitsune_make_vector
};

template<>
struct MappingTraits<FleCSIMetadata> {
   static void mapping(llvm::yaml::IO &io, FleCSIMetadata &c)
   {
      kitsune_debug("mapping(): MappingTraits<FleCSIMetadata>");

      #define kitsune_maprequired(name) io.mapRequired(#name, c.name)

      kitsune_maprequired(FlecsiRegisterTaskSimple);
      kitsune_maprequired(FlecsiRegisterTask);
      kitsune_maprequired(FlecsiRegisterMPITaskSimple);
      kitsune_maprequired(FlecsiRegisterMPITask);

      kitsune_maprequired(FlecsiExecuteTaskSimple);
      kitsune_maprequired(FlecsiExecuteTask);
      kitsune_maprequired(FlecsiExecuteMPITaskSimple);
      kitsune_maprequired(FlecsiExecuteMPITask);

      kitsune_maprequired(FlecsiRegisterDataClient);
      kitsune_maprequired(FlecsiRegisterField);
      kitsune_maprequired(FlecsiRegisterGlobal);
      kitsune_maprequired(FlecsiRegisterColor);

      /*
      // qqq
      io.mapRequired("handleGets",       c.handleGets);
      io.mapRequired("clientHandleGets", c.clientHandleGets);
      */

      #undef kitsune_maprequired
   }
};

} // namespace yaml
} // namespace llvm



// -----------------------------------------------------------------------------
// FleCSIAnalyzer::PreprocessorAnalyzer
// -----------------------------------------------------------------------------

namespace clang {
namespace sema {

struct FleCSIAnalyzer::PreprocessorAnalyzer : public clang::PPCallbacks
{
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

private:

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
         MacroData    // we'll save information about some Flecsi-related macro
      >
   > sourceMap_;

   // It seems that this gets used via Analyzer::md_, which is a reference
   // that initializes to metadata() (i.e., this).
   llvm::yaml::FleCSIMetadata metadata_;

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

      "flecsi_register_data_client",
      "flecsi_register_field",
      "flecsi_register_global",
      "flecsi_register_color"

      /*
      // qqq
      "flecsi_get_handle",
      "flecsi_get_client_handle"
      */
   }),

   sema_(sema),
   sourceMgr_(sema_.getSourceManager()),
   langOpts_(sema_.getLangOpts())
{
   kitsune_debug("PreprocessorAnalyzer::PreprocessorAnalyzer()");
}

// qqq Never called
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
   // The following can come in as any old token; we'll care about
   // and process it here only if it's from one of our macros
   const clang::Token &mactok,

   const clang::MacroDefinition &md,
   clang::SourceRange range,
   const clang::MacroArgs *args
) {
   kitsune_debug("PreprocessorAnalyzer::MacroExpands()");

   // Macro's identifier info
   const clang::IdentifierInfo *macinfo = mactok.getIdentifierInfo();

   // Macro's name
   const std::string macname = macinfo->getName().str();
   kitsune_print(macname);

   // If name isn't one of ours, we're done here
   if (flecsiMacros_.find(macname) == flecsiMacros_.end())
      return;

   // OK, we've recognized one of our macros; continue.
   // qqq Say more here.
   MacroData macdata;
   macdata.tok = mactok;

   // Number of arguments to the macro
   const std::size_t narg = args->getNumMacroArguments();
   kitsune_print(narg);

   // For each macro argument
   for (std::size_t arg = 0;  arg < narg;  ++arg) {
      // Pointer to first token of the argument (other tokens will follow)
      const clang::Token *const tokbegin = args->getUnexpArgument(arg);
      macdata.args.push_back(*tokbegin);

      // Number of tokens in the argument
      const std::size_t ntok = args->getArgLength(tokbegin);
      kitsune_print(ntok); (void)ntok;

      for (const clang::Token *t = tokbegin ; t->isNot(tok::eof);  ++t) {
         ///const clang::Token &t = *tokptr;
         kitsune_print(clang::Lexer::getSpelling(*t, sourceMgr_, langOpts_));
      }
   }

   const std::pair<clang::FileID, unsigned> &pos =
      sourceMgr_.getDecomposedExpansionLoc(mactok.getLocation());
   FileID   fileid = pos.first;
   unsigned offset = pos.second;
   sourceMap_[fileid].insert(std::make_pair(offset,macdata));

   /*
   Source Map:
      FileID --> Macro Data Map

   Macro Data Map:
      size_t --> Macro Data

   Macro Data:
      Token tok
      vector<Token> args

      string arg(i):
         return str(args[i])
      SourceLocation loc(i):
         return args[i].getLocation()
   */
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

   // Extract File ID and Offset from loc
   std::pair<FileID,unsigned> pos = sourceMgr_.getDecomposedExpansionLoc(loc);
   FileID   fileid = pos.first;
   unsigned offset = pos.second;

   // Look for File ID in our map of macro information
   auto itr = sourceMap_.find(fileid);
   if (itr == sourceMap_.end()) { // find failed
      kitsune_debug("getMacroData(): failure 1");
      return nullptr;
   }

   // For convenience: reference to nested map (Offset --> MacroData),
   // given that we just succeeded in finding File ID.
   std::map<
      std::size_t,
      MacroData
   > &um = itr->second;

   // Look for Offset in our (sub)map for the File ID
   auto mitr = um.lower_bound(offset);  // first element that's ">=" offset
   if (mitr == um.end()) { // find failed
      kitsune_debug("getMacroData(): failure 2");
      return nullptr;
   }

   // MacroData for the desired (FileID,Offset)
   MacroData &macdata = mitr->second;

   /**/
   kitsune_print(offset);
   kitsune_print(mitr->first);
   kitsune_print(macdata.tok.getLength());
   /**/

   if (offset > mitr->first + macdata.tok.getLength()) {
      kitsune_debug("getMacroData(): failure 3");
      return nullptr;
   }

   return &mitr->second;
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
public:
   Analyzer(
      clang::Sema &,
      clang::sema::FleCSIAnalyzer::PreprocessorAnalyzer *const
   );
  ~Analyzer();

   bool VisitVarDecl            (const clang::VarDecl  *const);
   bool VisitCallExpr           (clang::CallExpr *const);
   bool VisitTranslationUnitDecl(clang::TranslationUnitDecl *const);

///   std::uint32_t getFileLine     (const MacroData &, std::string &) const;
   std::string   getName         (const clang::NamedDecl *const);
   std::string   getQualifiedName(const clang::NamedDecl *const);

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

private:
   clang::Sema &sema_;
   clang::sema::FleCSIAnalyzer::PreprocessorAnalyzer *pa_;
   llvm::yaml::FleCSIMetadata &md_;  // FleCSIMetadata: see above
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

   std::string name = str(macdata.tok);
   kitsune_print(name);
   const clang::CXXRecordDecl *rd;



   // flecsi_register_task_simple(task, processor, launch)
   if (name == "flecsi_register_task_simple") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__",
        "register_task",
         3,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register task simple");
         llvm::yaml::FlecsiRegisterTaskSimple c(&sema_,&macdata);
         int pos = 0;

         c.task      = macdata.arg(pos++);
         c.processor = macdata.arg(pos++);
         c.launch    = macdata.arg(pos++);
         md_.FlecsiRegisterTaskSimple.push_back(c);

         kitsune_print(c.type     );
         kitsune_print(c.file     );
         kitsune_print(c.line     );
         kitsune_print(c.task     );
         kitsune_print(c.processor);
         kitsune_print(c.launch   );
      }
   }



   // flecsi_register_task(task, nspace, processor, launch)
   if (name == "flecsi_register_task") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__",
        "register_task",
         3,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register task");
         llvm::yaml::FlecsiRegisterTask c(&sema_,&macdata);
         int pos = 0;

         c.task      = macdata.arg(pos++);
         c.nspace    = macdata.arg(pos++);
         c.processor = macdata.arg(pos++);
         c.launch    = macdata.arg(pos++);
         md_.FlecsiRegisterTask.push_back(c);

         kitsune_print(c.type     );
         kitsune_print(c.file     );
         kitsune_print(c.line     );
         kitsune_print(c.task     );
         kitsune_print(c.nspace   );
         kitsune_print(c.processor);
         kitsune_print(c.launch   );
      }
   }



   // flecsi_register_mpi_task_simple(task)
   if (name == "flecsi_register_mpi_task_simple") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__",
        "register_task",
         3,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register mpi task simple");
         llvm::yaml::FlecsiRegisterMPITaskSimple c(&sema_,&macdata);
         int pos = 0;

         c.task = macdata.arg(pos++);
         md_.FlecsiRegisterMPITaskSimple.push_back(c);

         kitsune_print(c.type);
         kitsune_print(c.file);
         kitsune_print(c.line);
         kitsune_print(c.task);
      }
   }



   // flecsi_register_mpi_task(task, nspace)
   if (name == "flecsi_register_mpi_task") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::execution::task_interface__",
        "register_task",
         3,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register mpi task");
         llvm::yaml::FlecsiRegisterMPITask c(&sema_,&macdata);
         int pos = 0;

         c.task   = macdata.arg(pos++);
         c.nspace = macdata.arg(pos++);
         md_.FlecsiRegisterMPITask.push_back(c);

         kitsune_print(c.type);
         kitsune_print(c.file);
         kitsune_print(c.line);
         kitsune_print(c.task);
         kitsune_print(c.nspace);
      }
   }



   // flecsi_register_data_client(client_type, nspace, name)
   else if (name == "flecsi_register_data_client") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::data_client_interface__",
        "register_data_client",
         1,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register data client");

         const clang::CXXMethodDecl *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         const clang::QualType qt = getTypeArg(ta, 0);
         const clang::CXXRecordDecl *const cd = getClassDecl(qt);
         if (!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")) {
            sema_.Diag(
               macdata.loc(0),
               clang::diag::err_flecsi_not_a_data_client);
            return true;
         }

         llvm::yaml::FlecsiRegisterDataClient c(&sema_,&macdata);
         int pos = 0;

         c.client_type = macdata.arg(pos++);
         c.nspace      = macdata.arg(pos++);
         c.name        = macdata.arg(pos++);
         md_.FlecsiRegisterDataClient.push_back(c);
      }
   }



   // flecsi_register_field(client_type, nspace, name, data_type,
   //                       storage_class, versions, ...)
   else if (name == "flecsi_register_field") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::field_interface__",
        "register_field",
         1,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register field");

         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiRegisterField c(&sema_,&macdata);
         int pos = 0;

         c.client_type   = macdata.arg(pos++);
         c.nspace        = macdata.arg(pos++);
         c.name          = macdata.arg(pos++);
         c.data_type     = macdata.arg(pos++);
         c.storage_class = macdata.arg(pos++);
         c.versions      = getUIntArg(ta,5);
         ///c.indexSpace    = getUIntArg(ta,6);

         md_.FlecsiRegisterField.push_back(c);
      }
   }



   // flecsi_register_global(nspace, name, data_type, versions, ...)
   else if (name == "flecsi_register_global") {
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::field_interface__",
        "register_field",
         1,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register global");

         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiRegisterGlobal c(&sema_,&macdata);
         int pos = 0;

         c.nspace    = macdata.arg(pos++);
         c.name      = macdata.arg(pos++);
         c.data_type = macdata.arg(pos++);
         c.versions  = getUIntArg(ta,5);
         // qqq For an index_space as before, the macro seems to set things
         // up so that we have ...::global_is followed by __VA_ARGS__. Think
         // about how to deal with this. Should we just, for instance, save
         // the variadic stuff, and consider global_is to be implied by the
         // fact that we're dealing with (YAML-izing) this macro?
         // c.indexSpace = getUIntArg(ta,6);

         md_.FlecsiRegisterGlobal.push_back(c);
      }
   }



   // flecsi_register_color(nspace, name, data_type, versions, ...)
   else if (name == "flecsi_register_color") {
      kitsune_debug("almost found: register color");
      const clang::CallExpr *const call = getClassCall(
         var->getInit(),
        "flecsi::data::field_interface__",
        "register_field",
         1,
         rd
      );

      if (call != nullptr) {
         kitsune_debug("found: register color");

         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         llvm::yaml::FlecsiRegisterColor c(&sema_,&macdata);
         int pos = 0;

         c.nspace    = macdata.arg(pos++);
         c.name      = macdata.arg(pos++);
         c.data_type = macdata.arg(pos++);
         c.versions  = getUIntArg(ta,5);
         // c.indexSpace = getUIntArg(ta,6);

         md_.FlecsiRegisterColor.push_back(c);
      }
   }



   return true;
}



// -----------------------------------------------------------------------------
// VisitCallExpr
// -----------------------------------------------------------------------------

// qqq Remark: at the moment, this only ever returns true
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

   std::string name = str(macdata.tok);
   kitsune_print(name);
   const clang::CXXRecordDecl *rd;
   (void)rd;//qqq



   // flecsi_execute_task_simple(task, launch, ...)
   if (name == "flecsi_execute_task_simple") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__",
        "execute_task"
      )) {
         kitsune_debug("found: execute task simple");
         llvm::yaml::FlecsiExecuteTaskSimple c(&sema_,&macdata);
         int pos = 0;

         c.task   = macdata.arg(pos++);
         c.launch = macdata.arg(pos++);
         md_.FlecsiExecuteTaskSimple.push_back(c);

         kitsune_print(c.type  );
         kitsune_print(c.file  );
         kitsune_print(c.line  );
         kitsune_print(c.task  );
         kitsune_print(c.launch);
      }
   }



   // flecsi_execute_task(task, nspace, launch, ...)
   else if (name == "flecsi_execute_task") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__",
        "execute_task"
      )) {
         kitsune_debug("found: execute task");
         llvm::yaml::FlecsiExecuteTask c(&sema_,&macdata);
         int pos = 0;

         c.task   = macdata.arg(pos++);
         c.nspace = macdata.arg(pos++);
         c.launch = macdata.arg(pos++);
         md_.FlecsiExecuteTask.push_back(c);

         kitsune_print(c.type  );
         kitsune_print(c.file  );
         kitsune_print(c.line  );
         kitsune_print(c.task  );
         kitsune_print(c.nspace);
         kitsune_print(c.launch);
      }
   }



   // flecsi_execute_mpi_task_simple(task, ...)
   else if (name == "flecsi_execute_mpi_task_simple") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__",
        "execute_task"
      )) {
         kitsune_debug("found: execute mpi task simple");
         llvm::yaml::FlecsiExecuteMPITaskSimple c(&sema_,&macdata);
         int pos = 0;

         c.task = macdata.arg(pos++);
         md_.FlecsiExecuteMPITaskSimple.push_back(c);

         kitsune_print(c.type);
         kitsune_print(c.file);
         kitsune_print(c.line);
         kitsune_print(c.task);
      }
   }



   // flecsi_execute_mpi_task(task, nspace, ...)
   else if (name == "flecsi_execute_mpi_task") {
      if (getClassCall(
         call,
        "flecsi::execution::task_interface__",
        "execute_task"
      )) {
         kitsune_debug("found: execute mpi task");
         llvm::yaml::FlecsiExecuteMPITask c(&sema_,&macdata);
         int pos = 0;

         c.task   = macdata.arg(pos++);
         c.nspace = macdata.arg(pos++);
         md_.FlecsiExecuteMPITask.push_back(c);

         kitsune_print(c.type  );
         kitsune_print(c.file  );
         kitsune_print(c.line  );
         kitsune_print(c.task  );
         kitsune_print(c.nspace);
      }
   }


/*
   // flecsi_get_handle
   else if (name == "flecsi_get_handle") {
      if (getClassCall(
             call,
            "flecsi::data::field_data__",
            "get_handle", 1, rd)
      ) {
         llvm::yaml::GetHandle c;
         c.line   = getFileLine(macdata, c.file);
         // qqq Should these really be 1 and 2, or perhaps 0 and 1?
         c.nspace = macdata.arg(1);
         c.name   = macdata.arg(2);
         md_.handleGets.push_back(c);
      }
   }
*/

/*
   // flecsi_get_client_handle
   else if (name == "flecsi_get_client_handle") {
      if (getClassCall(
             call,
            "flecsi::data::client_data__",
            "get_client_handle", 0, rd
      )) {
         const clang::CXXMethodDecl *md = getMethod(call);
         const clang::TemplateArgumentList *templateArgs = getTemplateArgs(md);
         clang::QualType qt = getTypeArg(templateArgs, 0);
         const clang::CXXRecordDecl *cd = getClassDecl(qt);

         if (!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")) {
            sema_.Diag(macdata.loc(0), clang::diag::err_flecsi_not_a_data_client);
            return true;
         }

         llvm::yaml::GetClientHandle c;
         c.line     = getFileLine(macdata, c.file);
         c.meshType = macdata.arg(0);
         c.nspace   = macdata.arg(1);
         c.name     = macdata.arg(2);
         md_.clientHandleGets.push_back(c);
      }
   }
*/

   return true;
}



// -----------------------------------------------------------------------------
// VisitTranslationUnitDecl
// -----------------------------------------------------------------------------

bool Analyzer::VisitTranslationUnitDecl(clang::TranslationUnitDecl *const d)
{
   kitsune_debug("Analyzer::VisitTranslationUnitDecl()");

   for (auto di : d->decls())
      TraverseDecl(di);
   return true;
}



// ------------------------
///// getFileLine
// getName
// getQualifiedName
// ------------------------

/*
// Gets both the file and the line number. The file is returned in the second
// parameter, while the line number is returned as the function's return value.
std::uint32_t Analyzer::getFileLine(const MacroData &macdata, std::string &file) const
{
   kitsune_debug("Analyzer::getFileLine()");

   clang::SourceManager &srcMgr = sema_.getSourceManager();
   clang::SourceLocation loc = macdata.tok.getLocation();

   auto p = srcMgr.getDecomposedLoc(loc);
   file = srcMgr.getFilename(loc).str();

   return srcMgr.getLineNumber(p.first, p.second);
}
*/



std::string Analyzer::getName(const clang::NamedDecl *const nd)
{
   kitsune_debug("Analyzer::getName()");

   clang::DeclarationName dn = nd->getDeclName();
   if (!dn|| !dn.isIdentifier())
      return "";
   return dn.getAsString();
}



std::string Analyzer::getQualifiedName(const clang::NamedDecl *const nd)
{
   kitsune_debug("Analyzer::getQualifiedName()");

   return nd->getQualifiedNameAsString();
}



// ------------------------
// getTemplateArgs
// getClassDecl
// getMethod
// ------------------------

const clang::TemplateArgumentList *Analyzer::getTemplateArgs(
   const clang::FunctionDecl *const fd
) {
   kitsune_debug("Analyzer::getTemplateArgs()");

   return fd->getTemplateSpecializationArgs();
}



const clang::CXXRecordDecl *Analyzer::getClassDecl(clang::QualType qt)
{
   kitsune_debug("Analyzer::getClassDecl()");

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



const clang::CXXMethodDecl *
Analyzer::getMethod(const clang::CallExpr *const call)
{
   kitsune_debug("Analyzer::getMethod()");

   const clang::Decl *cd = call->getCalleeDecl();
   if (!cd)
      return nullptr;

   auto md = clang::dyn_cast<clang::CXXMethodDecl>(cd);
   if (!md)
      return nullptr;

   return md;
}



// ------------------------
// getIntArg
// getUIntArg
// getTypeArg
// ------------------------

std::int64_t Analyzer::getIntArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("Analyzer::getIntArg()");

   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getSExtValue();
}

std::uint64_t Analyzer::getUIntArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("Analyzer::getUIntArg()");

   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getZExtValue();
}

clang::QualType Analyzer::getTypeArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("Analyzer::getTypeArg()");

   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Type);
   return arg.getAsType();
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
   return getClassCall( // 6-arg
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
   return getClassCall( // 5-arg
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

   return getClassCall( // 6-arg
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
   ///std::ofstream ofs("/home/staley/llvm/f/.flecsi-analyzer");
   std::ofstream ofs(".flecsi-analyzer");

   // write string to file
   ofs << raw.str();
}



// destructor
FleCSIAnalyzer::~FleCSIAnalyzer()
{
   kitsune_debug("FleCSIAnalyzer::~FleCSIAnalyzer()");
}

} // namespace sema
} // namespace clang
