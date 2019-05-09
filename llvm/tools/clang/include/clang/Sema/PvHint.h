//===--- LoopHint.h - Types for LoopHint ------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_SEMA_PVHINT_H
#define LLVM_CLANG_SEMA_PVHINT_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/AttributeList.h"
#include "clang/Sema/Ownership.h"

namespace clang {

/// \brief Loop optimization hint for loop and unroll pragmas.
struct PvHint {
  // Source range of the directive.
  SourceRange Range;
  // Identifier corresponding to the name of the pragma.  "loop" for
  // "#pragma clang loop" directives and "unroll" for "#pragma unroll"
  // hints.
  IdentifierLoc *PragmaNameLoc;
  // Name of the loop hint.  Examples: "unroll", "vectorize".  In the
  // "#pragma unroll" and "#pragma nounroll" cases, this is identical to
  // PragmaNameLoc.
  IdentifierLoc *OptionLoc;
  // Identifier for the hint state argument.  If null, then the state is
  // default value such as for "#pragma unroll".
  IdentifierLoc *StateLoc;

  IdentifierLoc *GatherVal;
  IdentifierLoc *ScatterVal;
  IdentifierLoc *IndexVal;

  Expr *BufferSize;
  Expr *ListSize;

  PvHint()
      : PragmaNameLoc(nullptr), OptionLoc(nullptr), StateLoc(nullptr), GatherVal(nullptr),  
        ScatterVal(nullptr), IndexVal(nullptr), BufferSize(nullptr), ListSize(nullptr) {}
};

} // end namespace clang

#endif // LLVM_CLANG_SEMA_LOOPHINT_H

