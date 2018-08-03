
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/Support/YAMLTraits.h"

#include "clang/Sema/Kitsune/FleCSIPreprocessorASTVisitor.h"
#include "clang/Sema/Kitsune/FleCSIPreprocessor.h"

#ifdef KITSUNE_YAML
#include <fstream>
#endif



// -----------------------------------------------------------------------------
// Helper: greatest_less_or_equal
// -----------------------------------------------------------------------------

namespace flecsi {

// const
template<class MAP>
typename MAP::const_iterator greatest_less_or_equal(
   const MAP &m,
   const typename MAP::key_type &k
) {
    typename MAP::const_iterator it = m.upper_bound(k);
    return it == m.begin() ? m.end() : --it;
}

// non-const
template<class MAP>
typename MAP::iterator greatest_less_or_equal(
   MAP &m,
   const typename MAP::key_type &k
) {
    typename MAP::iterator it = m.upper_bound(k);
    return it == m.begin() ? m.end() : --it;
}

}



// -----------------------------------------------------------------------------
// Preprocessor
// Member functions
// -----------------------------------------------------------------------------

namespace flecsi {

// ------------------------
// Constructor
// Destructor
// ------------------------

Preprocessor::Preprocessor(clang::Sema &_sema)
 : sema(_sema),
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
      //"flecsi_get_handles",
      //"flecsi_get_handles_all",

      "flecsi_get_mutator",
      "flecsi_get_global",
      "flecsi_get_color"
   })
{
   kitsune_debug("Preprocessor::Preprocessor()");
}


Preprocessor::~Preprocessor()
{
   kitsune_debug("Preprocessor::~Preprocessor()");
}



// ------------------------
// MacroDefined
// MacroExpands
// These override the ones
// in the base PPCallbacks
// ------------------------

// MacroDefined
void Preprocessor::MacroDefined(
   const clang::Token &token,
   const clang::MacroDirective *
) {
   kitsune_debug("Preprocessor::MacroDefined()");
   kitsune_debug(
      std::string("   token.getName() == \"") + token.getName() + '"');
}



// MacroExpands
void Preprocessor::MacroExpands(
   const clang::Token &mactok,
   const clang::MacroDefinition &def,
         clang::SourceRange range,
   const clang::MacroArgs *args
) {
   kitsune_debug("Preprocessor::MacroExpands()");

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
   // Build a MacroInvocation object for this particular macro call.
   MacroInvocation invoke;
   invoke.token = mactok;
   invoke.end = sema.getSourceManager().
      getDecomposedExpansionLoc(range.getEnd()).second;

   // Number of arguments to the macro
   const std::size_t narg = args->getNumMacroArguments();
   kitsune_print(narg);

   // For each macro argument
   for (std::size_t a = 0;  a < narg;  ++a) {
      // Pointer to first token of the argument (other tokens will follow)
      const clang::Token *const tokbegin = args->getUnexpArgument(a);

      invoke.args.push_back(std::vector<clang::Token>{});

      // For each token of the current macro argument
      for (const clang::Token *t = tokbegin;  t->isNot(clang::tok::eof);  ++t)
         invoke.args[a].push_back(*t);
   }

   // Enter the new MacroInvocation object into our map structure,
   // essentially a map(FileID,map(Offset,MacroInvocation)).
   const std::pair<clang::FileID, unsigned> &pos =
      sema.getSourceManager().getDecomposedExpansionLoc(mactok.getLocation());
   clang::FileID fileid = pos.first;
   unsigned offset = pos.second;
   sourceMap_[fileid].insert(std::make_pair(offset,invoke));
}



// ------------------------
// getInvocation
// ------------------------

MacroInvocation *Preprocessor::getInvocation(
   const clang::SourceLocation loc
) {
   kitsune_debug("Preprocessor::getInvocation()");

   // For the construct (declaration, expression, etc.) we're examining, get
   // its File ID and offset. We'll then see if it's associated with a macro.
   std::pair<clang::FileID,unsigned>
      pos = sema.getSourceManager().getDecomposedExpansionLoc(loc);
   clang::FileID fileid = pos.first;
   unsigned offset = pos.second;

   // Look for File ID in our map of macro information
   auto itr = sourceMap_.find(fileid);
   if (itr == sourceMap_.end()) { // find failed
      kitsune_debug("getInvocation(): failure 1");
      return nullptr;
   }

   // Submap (offset --> MacroInvocation) for the File ID.
   // Keys of this submap are offsets of the starts of macro invocations.
   std::map<std::size_t, MacroInvocation> &sub = itr->second;

   // Look for the largest submap entry <= offset. That macro would
   // be responsible for the present construct, if *any* macro is.
   auto mitr = greatest_less_or_equal(sub,offset);
   if (mitr == sub.end()) { // find failed; offset must be before any macro
      kitsune_debug("getInvocation(): failure 2");
      return nullptr;
   }

   // MacroInvocation for the desired (FileID,offset)
   const MacroInvocation  &invoke = mitr->second;
   const std::size_t begin = mitr->first;
   const std::size_t end   = invoke.end;

   if (begin <= offset && offset <= end)
      return &mitr->second;

   kitsune_debug("getInvocation(): failure 3");
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
   raw.flush(); // <-- necessary

   // open file
   // qqq Eventually, figure out where this goes (w/o the full path,
   // which we obviously don't really want).
   std::ofstream ofs("/home/staley/llvm/f/.flecsi-analyzer");

   // write string to file
   ofs << raw.str();
#endif
}

}
