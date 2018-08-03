
#ifndef FleCSIPreprocessorASTVisitor
#define FleCSIPreprocessorASTVisitor

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
// PreprocessorASTVisitor
// -----------------------------------------------------------------------------

namespace flecsi {

class Preprocessor;

class PreprocessorASTVisitor
 : public clang::RecursiveASTVisitor<PreprocessorASTVisitor>
{
   clang::Sema  &sema;
   Preprocessor &prep;

public:

   // constructor
   PreprocessorASTVisitor(clang::Sema &_sema, Preprocessor &_prep)
    : sema(_sema),
      prep(_prep)
   {
      kitsune_debug("PreprocessorASTVisitor::PreprocessorASTVisitor()");
   }

   // destructor
  ~PreprocessorASTVisitor()
   {
      kitsune_debug("PreprocessorASTVisitor::~PreprocessorASTVisitor()");
   }

   // callbacks for clang::RecursiveASTVisitor; definitions are in .cpp
   bool VisitVarDecl      (const clang::VarDecl       *const);
   bool VisitCallExpr     (const clang::CallExpr      *const);
   bool VisitTypeAliasDecl(const clang::TypeAliasDecl *const);
};

}

#endif
