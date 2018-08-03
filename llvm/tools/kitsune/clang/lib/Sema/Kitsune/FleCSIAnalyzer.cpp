
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/Support/YAMLTraits.h"

#include "clang/Sema/Kitsune/FleCSIAnalyzer.h"



// -----------------------------------------------------------------------------
// flecsi::analyzer member functions
// -----------------------------------------------------------------------------

namespace flecsi {

// constructor
analyzer::analyzer(clang::Sema &_sema)
 : sema(_sema),
   preprocessor(new Preprocessor(sema))
{
   kitsune_debug("analyzer::analyzer()");

   // re: preprocessor
   sema.getPreprocessor().addPPCallbacks(
      std::unique_ptr<Preprocessor>(preprocessor)
   );
}

// destructor
analyzer::~analyzer()
{
   kitsune_debug("analyzer::~analyzer()");
}



// instance
analyzer &analyzer::instance(clang::Sema *const sema)
{
   static bool firstcall = true;
   if (firstcall) {
      assert(sema != nullptr);
      firstcall = false;
   }
   static analyzer obj(*sema);
   return obj;
}



// analyze
void analyzer::analyze(clang::Decl &decl)
{
   kitsune_debug("analyzer::analyze()");

   // re: preprocessor
   preprocessor->analyze(decl);
}

// finalize
void analyzer::finalize()
{
   kitsune_debug("analyzer::finalize()");

#ifdef KITSUNE_YAML
   // re: preprocessor
   preprocessor->finalize();
#endif
}

}
