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

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/MacroArgs.h"

#include "clang/Sema/Kitsune/FleCSIPreprocessorASTVisitor.h"
#include "clang/Sema/Kitsune/FleCSIPreprocessor.h"

#ifdef KITSUNE_YAML
#include <fstream>
#endif

namespace flecsi {



// -----------------------------------------------------------------------------
// Helper: greatest_less_than_or_equal_to
// -----------------------------------------------------------------------------

// const
template<class MAP>
inline typename MAP::const_iterator greatest_less_than_or_equal_to(
   const MAP &m,
   const typename MAP::key_type &k
) {
    typename MAP::const_iterator it = m.upper_bound(k);
    return it == m.begin() ? m.end() : --it;
}

// non-const
template<class MAP>
inline typename MAP::iterator greatest_less_than_or_equal_to(
   MAP &m,
   const typename MAP::key_type &k
) {
    typename MAP::iterator it = m.upper_bound(k);
    return it == m.begin() ? m.end() : --it;
}



// -----------------------------------------------------------------------------
// Preprocessor::macros
// Initialization
// -----------------------------------------------------------------------------

const std::set<std::string> Preprocessor::macros {
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
   // apparently deprecated...
   // "flecsi_get_handles",
   // "flecsi_get_handles_all",

   "flecsi_get_mutator",
   "flecsi_get_global",
   "flecsi_get_color"
};



// -----------------------------------------------------------------------------
// Preprocessor
// Member functions
// -----------------------------------------------------------------------------

// ------------------------
// Constructor
// Destructor
// ------------------------

Preprocessor::Preprocessor(clang::Sema &s) : sema(s)
{
   kitsune_debug("Preprocessor::Preprocessor()");
}


Preprocessor::~Preprocessor()
{
   kitsune_debug("Preprocessor::~Preprocessor()");
}



// ------------------------
// PPCallbacks overrides:
// MacroDefined
// MacroExpands
// ------------------------

// MacroDefined
// Not actually doing anything useful at the moment.
// Consider removing, at some point.
void Preprocessor::MacroDefined(
   const clang::Token &token,
   const clang::MacroDirective *const
) {
   kitsune_debug("Preprocessor::MacroDefined()");
   kitsune_debug(std::string("   token.getName() == ") + token.getName());
}



// MacroExpands
void Preprocessor::MacroExpands(
   const clang::Token &token,
   const clang::MacroDefinition &def,
   const clang::SourceRange range,
   const clang::MacroArgs *const args
) {
   kitsune_debug("Preprocessor::MacroExpands()");

   // Macro's identifier info
   const clang::IdentifierInfo *const ii = token.getIdentifierInfo();
   if (!ii) return;

   // Macro's name
   const std::string name = ii->getName().str();
   kitsune_print(name);

   // If name isn't one of ours, we're done here
   if (macros.find(name) == macros.end())
      return;

   // OK, we've recognized one of our macros.
   // Build a MacroInvocation object for this particular macro call.
   MacroInvocation macro;
   macro.token = token;
   macro.end = sema.getSourceManager().
      getDecomposedExpansionLoc(range.getEnd()).second;

   // Number of arguments to the macro
   const std::size_t narg = args->getNumMacroArguments();
   kitsune_print(narg);

   // For each macro argument
   for (std::size_t a = 0;  a < narg;  ++a) {
      // Pointer to first token of the argument (other tokens will follow)
      const clang::Token *const tokbegin = args->getUnexpArgument(a);

      macro.args.push_back(std::vector<clang::Token>{});

      // For each token of the current macro argument
      for (const clang::Token *t = tokbegin;  t->isNot(clang::tok::eof);  ++t)
         macro.args[a].push_back(*t);
   }

   // Enter the new MacroInvocation object into our map structure,
   // essentially a map(FileID,map(Offset,MacroInvocation)).
   const std::pair<clang::FileID, unsigned> &pos =
      sema.getSourceManager().getDecomposedExpansionLoc(token.getLocation());
   const clang::FileID fileid = pos.first;
   const unsigned offset = pos.second;
   sourceMap[fileid].insert(std::make_pair(offset,macro));
}



// ------------------------
// invocation
// ------------------------

const MacroInvocation *Preprocessor::invocation(
   const clang::SourceLocation &loc
) const {
   kitsune_debug("Preprocessor::invocation()");

   // For the construct (declaration, expression, etc.) we're examining, get
   // its File ID and offset. We'll then see if it's associated with a macro.
   const std::pair<clang::FileID,unsigned>
      pos = sema.getSourceManager().getDecomposedExpansionLoc(loc);
   const clang::FileID fileid = pos.first;
   const unsigned offset = pos.second;

   // Look for File ID in our map of macro information
   auto itr = sourceMap.find(fileid);
   if (itr == sourceMap.end()) { // find failed
      kitsune_debug("invocation(): failure 1");
      return nullptr;
   }

   // Submap (offset --> MacroInvocation) for the File ID.
   // Keys of this submap are offsets of the starts of macro invocations.
   const std::map<std::size_t, MacroInvocation> &sub = itr->second;

   // Look for the largest submap entry <= offset. That macro would
   // be responsible for the present construct, if *any* macro is.
   auto mitr = greatest_less_than_or_equal_to(sub,offset);
   if (mitr == sub.end()) { // find failed; offset must be before any macro
      kitsune_debug("invocation(): failure 2");
      return nullptr;
   }

   // MacroInvocation for the desired (FileID,offset)
   const MacroInvocation  &macro = mitr->second;
   const std::size_t begin = mitr->first;
   const std::size_t end   = macro.end;

   if (begin <= offset && offset <= end)
      return &mitr->second;

   kitsune_debug("invocation(): failure 3");
   return nullptr;
}



// ------------------------
// analyze
// finalize
// ------------------------

void Preprocessor::analyze(clang::Decl &decl)
{
   PreprocessorASTVisitor visitor(sema,*this);
   visitor.TraverseDecl(&decl);
}



void Preprocessor::finalize()
{
#ifdef KITSUNE_YAML
   // write yaml to string
   std::string str;
   llvm::raw_string_ostream raw(str);
   llvm::yaml::Output yout(raw);
   yout << yaml();
   raw.flush(); // <-- necessary :-/

   // open file
   // FIXME
   // Eventually, figure out where this goes (w/o the full path,
   // which we obviously don't really want).
   std::ofstream ofs("/home/staley/llvm/f/.flecsi-analyzer");

   // write string to file
   ofs << raw.str();
#endif
}

}
