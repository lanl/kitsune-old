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

#ifndef FleCSIPreprocessor
#define FleCSIPreprocessor

#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/SemaDiagnostic.h"

#include "clang/Sema/Kitsune/FleCSIPreprocessorYAML.h"

// tostr: helper function
// Get a token's spelling.
namespace {
inline std::string tostr(
   const clang::Sema  &sema,
   const clang::Token &token
) {
   return clang::Lexer::getSpelling(
      token,
      sema.getSourceManager(),
      sema.getLangOpts()
   );
}
}



// -----------------------------------------------------------------------------
// MacroInvocation
// Contains information regarding a particular preprocessor-macro invocation.
// -----------------------------------------------------------------------------

namespace flecsi {

class MacroInvocation {
public:

   // ------------------------
   // data
   // ------------------------

   // Token for the macro's name
   clang::Token token;

   // Offset of the macro invocation's end, i.e. the ")" in "macro(foo,bar,...)"
   std::size_t end;

   // Arguments
   // Each argument consists of some number of tokens
   std::vector<std::vector<clang::Token>> args;

   // ------------------------
   // functions
   // ------------------------

   // Number of arguments to the macro
   std::size_t size() const
   {
      return args.size();
   }

   // Source-code location of argument [a], token [t]
   clang::SourceLocation loc(
      const std::size_t a,
      const std::size_t t
   ) const {
      kitsune_assert(a < args.size());
      kitsune_assert(t < args[a].size());
      return args[a][t].getLocation();
   }

   // String representation of argument [a], token [t]
   std::string str(
      const clang::Sema &sema,
      const std::size_t a,
      const std::size_t t
   ) const {
      kitsune_assert(a < args.size());
      kitsune_assert(t < args[a].size());
      return tostr(sema,args[a][t]);
   }

   // String representation of argument [a], with all tokens put together,
   // separated by spaces
   std::string str(
      const clang::Sema &sema,
      const std::size_t a
   ) const {
      kitsune_assert(a < args.size());
      std::string s;
      for (std::size_t t = 0;  t < args[a].size();  ++t)
         s += (t ? " " : "") + str(sema,a,t);
      return s;
   }
};

}



// -----------------------------------------------------------------------------
// Preprocessor
// -----------------------------------------------------------------------------

namespace flecsi {

class Preprocessor : public clang::PPCallbacks
{
   // FleCSI macros that we'll recognize
   static const std::set<std::string> macros;

   // Sema, from the overall FleCSI Analyzer
   clang::Sema &sema;

   // FleCSI macro information
   // Idea: For some file, at some location, we'll store information
   // about a FleCSI-related macro invocation
   std::map<
      clang::FileID,
      std::map<
         std::size_t,
         MacroInvocation  // token, end, args
      >
   > sourceMap;

   // data collected
   PreprocessorYAML YAML;

public:

   // constructor, destructor
   Preprocessor(clang::Sema &);
  ~Preprocessor();

   // PPCallbacks overrides
   void MacroDefined(
      const clang::Token &,
      const clang::MacroDirective *const
   ) override;

   void MacroExpands(
      const clang::Token &,
      const clang::MacroDefinition &,
      const clang::SourceRange,
      const clang::MacroArgs *const
   ) override;

   // accessors
   const MacroInvocation *invocation(const clang::SourceLocation &) const;
   PreprocessorYAML &yaml() { return YAML; }

   // analyze, finalize
   void analyze(clang::Decl &);
   void finalize();
};

}

#endif
