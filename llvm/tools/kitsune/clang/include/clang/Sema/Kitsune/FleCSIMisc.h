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

#ifndef FleCSIMisc
#define FleCSIMisc

#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/Support/YAMLTraits.h"

// Write YAML file?
#define KITSUNE_YAML

// Insert debugging output?
#define KITSUNE_DEBUG

#ifdef KITSUNE_DEBUG
   #include <iostream>
   /// #include <typeinfo>
   /// #include <boost/core/demangle.hpp>
#endif



// -----------------------------------------------------------------------------
// kitsune_debug (function)
// kitsune_print (macro)
// -----------------------------------------------------------------------------

// kitsune_debug
// We'll put this in a plain unnamed namespace so that it's easy
// to use from anywhere, e.g. flecsi or llvm::yaml.
namespace {

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

}

// kitsune_print
#ifdef KITSUNE_DEBUG
   #define kitsune_print(x) \
      std::cout << "kitsune: " #x " == " << (x) << std::endl
#else
   #define kitsune_print(x)
#endif



// -----------------------------------------------------------------------------
// getFileLine: declaration
// -----------------------------------------------------------------------------

namespace flecsi {

std::pair<std::string,std::uint32_t> getFileLine(
   const clang::Sema  &sema,
   const clang::Token &token
);

}



// -----------------------------------------------------------------------------
// SequenceTraits<vector<T>>
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

template<class T, class ALLOCATOR>
class SequenceTraits<std::vector<T,ALLOCATOR>> {
public:

   // size
   static std::size_t size(
      IO &, std::vector<T,ALLOCATOR> &vec
   ) {
      kitsune_debug("size()");
      return vec.size();
   }

   // element
   static T &element(
      IO &, std::vector<T,ALLOCATOR> &vec,
      const std::size_t index
   ) {
      kitsune_debug("element()");
      return vec[index];
   }
};

}
}

#endif
