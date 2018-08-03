
#ifndef FleCSIAnalyzer
#define FleCSIAnalyzer

#include "clang/Sema/Kitsune/FleCSIPreprocessor.h"



// -----------------------------------------------------------------------------
// analyzer
// Implemented as a singleton
// -----------------------------------------------------------------------------

namespace flecsi {

class analyzer {
   clang::Sema &sema;

   // elements of the FleCSI analysis; for now,
   // just examine preprocessor-related constructs
   Preprocessor *const preprocessor;

   // singleton ==> private ctor/dtor
   analyzer(clang::Sema &);
  ~analyzer();

   // singleton ==> private assignment
   analyzer &operator=(const analyzer &) { return *this; }

public:

   // instance of singleton
   static analyzer &instance(clang::Sema *const = nullptr);

   // analyze an individual Decl
   void analyze(clang::Decl &);

   // finalize the overall analysis
   void finalize();
};

}

#endif
