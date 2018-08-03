
#include "clang/Sema/Kitsune/FleCSIMisc.h"

// -----------------------------------------------------------------------------
// getFileLine
// Get a token's file name and line number
// -----------------------------------------------------------------------------

namespace flecsi {

std::pair<std::string,std::uint32_t> getFileLine(
   const clang::Sema  &sema,
   const clang::Token &token
) {
   kitsune_debug("getFileLine()");

   clang::SourceManager &mgr = sema .getSourceManager();
   clang::SourceLocation loc = token.getLocation();

   auto p = mgr.getDecomposedLoc(loc);

   return std::pair<std::string,std::uint32_t>(
      mgr.getFilename(loc).str(),
      mgr.getLineNumber(p.first, p.second)
   );
}

}
