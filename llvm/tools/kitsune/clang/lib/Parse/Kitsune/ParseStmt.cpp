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

#include "clang/Parse/Parser.h"

#include "clang/Basic/Attributes.h"
#include "clang/Basic/PrettyStackTrace.h"
#include "clang/Parse/RAIIObjectsForParser.h"
#include "clang/Sema/DeclSpec.h"
#include "clang/Sema/LoopHint.h"
#include "clang/Sema/PrettyDeclStackTrace.h"
#include "clang/Sema/Scope.h"
#include "clang/Sema/TypoCorrection.h"

#include <iostream>
#define np(X)                                                              \
  std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__  \
            << ": " << #X << " = " << (X) << std::endl

using namespace clang;

StmtResult Parser::ParseForAllStatement(SourceLocation *TrailingElseLoc) {
  assert(Tok.is(tok::kw_forall) && "Not a forall stmt!");
  SourceLocation ForallLoc = ConsumeToken();  // eat the 'forall'.

  if (Tok.isNot(tok::l_paren)) {
    Diag(Tok, diag::err_expected_lparen_after) << "forall";
    SkipUntil(tok::semi);
    return StmtError();
  }

  bool C99orCXXorObjC = getLangOpts().C99 || getLangOpts().CPlusPlus ||
    getLangOpts().ObjC1;

  unsigned ScopeFlags = 0;
  if (C99orCXXorObjC)
    ScopeFlags = Scope::DeclScope | Scope::ControlScope;

  ParseScope ForallScope(this, ScopeFlags);

  BalancedDelimiterTracker T(*this, tok::l_paren);
  
  SourceLocation LP = Tok.getLocation();
  T.consumeOpen();

  ParsedAttributesWithRange attrs(AttrFactory);
  MaybeParseCXX11Attributes(attrs);

  SourceLocation DeclStart = Tok.getLocation(), DeclEnd;

  DeclGroupPtrTy DG = ParseSimpleDeclaration(
      Declarator::ForContext, DeclEnd, attrs, false, nullptr);

  VarDecl* LoopVar = dyn_cast<VarDecl>(DG.get().getSingleDecl());
  assert(LoopVar);

  StmtResult InitRes = 
    Actions.ActOnDeclStmt(DG, DeclStart, Tok.getLocation());

  ConsumeToken();

  ExprResult CondRes = ParseExpression();
  if(CondRes.isInvalid()){
    assert(false);
  }

  assert(Tok.is(tok::semi));
  ConsumeToken();

  ExprResult IncRes = ParseExpression();
  if(IncRes.isInvalid()){
    assert(false);
  }

  SourceLocation RP = Tok.getLocation();
  T.consumeClose();

  ParseScope InnerScope(this, Scope::DeclScope, C99orCXXorObjC,
                        Tok.is(tok::l_brace));

  if (C99orCXXorObjC)
    getCurScope()->decrementMSManglingNumber();

  StmtResult BodyRes(ParseStatement(TrailingElseLoc));

  InnerScope.Exit();

  ForallScope.Exit();

  if (BodyRes.isInvalid())
    return StmtError();

  return Actions.ActOnForallStmt(InitRes.get(), LoopVar, CondRes.get(),
                                 IncRes.get(), BodyRes.get(),
                                 ForallLoc, T.getOpenLocation(),
                                 T.getCloseLocation());
}
