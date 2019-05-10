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
  IdentifierLoc *PragmaNameLoc;
  IdentifierLoc *GatherVal;
  IdentifierLoc *ScatterVal;
  IdentifierLoc *IndexVal;

  Expr *BufferSize;
  Expr *ListSize;

  PvHint()
      : PragmaNameLoc(nullptr), GatherVal(nullptr),  
        ScatterVal(nullptr), IndexVal(nullptr), BufferSize(nullptr), ListSize(nullptr) {}
};

} // end namespace clang

#endif // LLVM_CLANG_SEMA_PVHINT_H

