
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
#define noKITSUNE_YAML

// Insert debugging output?
#define noKITSUNE_DEBUG

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
