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

  unsigned ScopeFlags = Scope::DeclScope | Scope::ControlScope;

  ParseScope ForallScope(this, ScopeFlags);

  BalancedDelimiterTracker T(*this, tok::l_paren);
  
  SourceLocation LP = Tok.getLocation();
  T.consumeOpen();

  ExprResult Value;

  bool ForEach = false, ForRange = false;
  StmtResult FirstPart;
  Sema::ConditionResult SecondPart;
  ExprResult Collection;
  ForRangeInit ForRangeInit;
  FullExprArg ThirdPart(Actions);

  if (Tok.is(tok::code_completion)) {
    Actions.CodeCompleteOrdinaryName(getCurScope(), Sema::PCC_ForInit);
    cutOffParsing();
    return StmtError();
  }

  ParsedAttributesWithRange attrs(AttrFactory);
  MaybeParseCXX11Attributes(attrs);

  // Parse the first part of the for specifier.
  if (Tok.is(tok::semi)) {
    ProhibitAttributes(attrs);
    // We disallow this syntax for now.
    Diag(Tok, diag::err_kitsune_for_missing_control_variable) << ";";
    ConsumeToken();
  } else if (getLangOpts().CPlusPlus && Tok.is(tok::identifier) &&
             isForRangeIdentifier()) {
    ProhibitAttributes(attrs);
    IdentifierInfo *Name = Tok.getIdentifierInfo();
    SourceLocation Loc = ConsumeToken();
    MaybeParseCXX11Attributes(attrs);

    ForRangeInit.ColonLoc = ConsumeToken();
    if (Tok.is(tok::l_brace))
      ForRangeInit.RangeExpr = ParseBraceInitializer();
    else
      ForRangeInit.RangeExpr = ParseExpression();

    Diag(Loc, diag::err_for_range_identifier)
      << ((getLangOpts().CPlusPlus11 && !getLangOpts().CPlusPlus1z)
              ? FixItHint::CreateInsertion(Loc, "auto &&")
              : FixItHint());

    FirstPart = Actions.ActOnCXXForRangeIdentifier(getCurScope(), Loc, Name,
                                                   attrs, attrs.Range.getEnd());
    ForRange = true;
  } else if (isForInitDeclaration()) {  // _Cilk_for (int X = 4;
    // Parse declaration, which eats the ';'.
    if (!C99orCXXorObjC)   // Use of C99-style for loops in C90 mode?
      Diag(Tok, diag::ext_c99_variable_decl_in_for_loop);

    // In C++0x, "for (T NS:a" might not be a typo for ::
    bool MightBeForRangeStmt = getLangOpts().CPlusPlus;
    ColonProtectionRAIIObject ColonProtection(*this, MightBeForRangeStmt);

    SourceLocation DeclStart = Tok.getLocation(), DeclEnd;
    DeclGroupPtrTy DG = ParseSimpleDeclaration(
        Declarator::ForContext, DeclEnd, attrs, false,
        MightBeForRangeStmt ? &ForRangeInit : nullptr);
    FirstPart = Actions.ActOnDeclStmt(DG, DeclStart, Tok.getLocation());
    if (ForRangeInit.ParsedForRangeDecl()) {
      Diag(ForRangeInit.ColonLoc, getLangOpts().CPlusPlus11 ?
           diag::warn_cxx98_compat_for_range : diag::ext_for_range);

      ForRange = true;
    } else if (Tok.is(tok::semi)) {  // for (int x = 4;
      ConsumeToken();
    } else if ((ForEach = isTokIdentifier_in())) {
      Actions.ActOnForEachDeclStmt(DG);
      // ObjC: for (id x in expr)
      ConsumeToken(); // consume 'in'

      if (Tok.is(tok::code_completion)) {
        Actions.CodeCompleteObjCForCollection(getCurScope(), DG);
        cutOffParsing();
        return StmtError();
      }
      Collection = ParseExpression();
    } else {
      Diag(Tok, diag::err_expected_semi_for);
    }
  } else {
    ProhibitAttributes(attrs);
    Value = Actions.CorrectDelayedTyposInExpr(ParseExpression());

    ForEach = isTokIdentifier_in();

    // Turn the expression into a stmt.
    if (!Value.isInvalid()) {
      if (ForEach)
        FirstPart = Actions.ActOnForEachLValueExpr(Value.get());
      else
        FirstPart = Actions.ActOnExprStmt(Value);
    }

    if (Tok.is(tok::semi)) {
      ConsumeToken();
    } else if (ForEach) {
      ConsumeToken(); // consume 'in'

      if (Tok.is(tok::code_completion)) {
        Actions.CodeCompleteObjCForCollection(getCurScope(), nullptr);
        cutOffParsing();
        return StmtError();
      }
      Collection = ParseExpression();
    } else if (getLangOpts().CPlusPlus11 && Tok.is(tok::colon) && FirstPart.get()) {
      // User tried to write the reasonable, but ill-formed, for-range-statement
      //   for (expr : expr) { ... }
      Diag(Tok, diag::err_for_range_expected_decl)
        << FirstPart.get()->getSourceRange();
      SkipUntil(tok::r_paren, StopBeforeMatch);
      SecondPart = Sema::ConditionError();
    } else {
      if (!Value.isInvalid()) {
        Diag(Tok, diag::err_expected_semi_for);
      } else {
        // Skip until semicolon or rparen, don't consume it.
        SkipUntil(tok::r_paren, StopAtSemi | StopBeforeMatch);
        if (Tok.is(tok::semi))
          ConsumeToken();
      }
    }
  }

  // Parse the second part of the for specifier.
  getCurScope()->AddFlags(Scope::BreakScope | Scope::ContinueScope);
  if (!ForEach && !ForRange && !SecondPart.isInvalid()) {
    // Parse the second part of the for specifier.
    if (Tok.is(tok::semi)) {  // for (...;;
      // no second part.
      Diag(Tok, diag::err_kitsune_for_missing_condition)
        << FirstPart.get()->getSourceRange();
    } else if (Tok.is(tok::r_paren)) {
      // missing both semicolons.
      Diag(Tok, diag::err_kitsune_for_missing_condition)
        << FirstPart.get()->getSourceRange();
    } else {
      if (getLangOpts().CPlusPlus)
        SecondPart =
            ParseCXXCondition(nullptr, ForallLoc, Sema::ConditionKind::Boolean);
      else {
        ExprResult SecondExpr = ParseExpression();
        if (SecondExpr.isInvalid())
          SecondPart = Sema::ConditionError();
        else
          SecondPart =
              Actions.ActOnCondition(getCurScope(), ForallLoc, SecondExpr.get(),
                                     Sema::ConditionKind::Boolean);
      }
    }

    if (Tok.isNot(tok::semi)) {
      if (!SecondPart.isInvalid())
        Diag(Tok, diag::err_expected_semi_for);
      else
        // Skip until semicolon or rparen, don't consume it.
        SkipUntil(tok::r_paren, StopAtSemi | StopBeforeMatch);
    }

    if (Tok.is(tok::semi)) {
      ConsumeToken();
    }

    // Parse the third part of the _Cilk_for specifier.
    if (Tok.isNot(tok::r_paren)) {   // for (...;...;)
      ExprResult Third = ParseExpression();
      // FIXME: The C++11 standard doesn't actually say that this is a
      // discarded-value expression, but it clearly should be.
      ThirdPart = Actions.MakeFullDiscardedValueExpr(Third.get());
    } else
      Diag(Tok, diag::err_kitsune_for_missing_increment)
        << FirstPart.get()->getSourceRange();      
  }
  // Match the ')'.
  T.consumeClose();

  // // C++ Coroutines [stmt.iter]:
  // //   'co_await' can only be used for a range-based for statement.
  // if (CoawaitLoc.isValid() && !ForRange) {
  //   Diag(CoawaitLoc, diag::err_for_co_await_not_range_for);
  //   CoawaitLoc = SourceLocation();
  // }

  // // We need to perform most of the semantic analysis for a C++0x for-range
  // // statememt before parsing the body, in order to be able to deduce the type
  // // of an auto-typed loop variable.
  // StmtResult ForRangeStmt;
  // StmtResult ForEachStmt;

  // TODO: Extend _Cilk_for to support these.
  if (ForRange) {
    //Diag(ForallLoc, diag::err_kitsune_for_forrange_loop_not_supported);
    // ForRangeStmt = Actions.ActOnCXXForRangeStmt(ForallLoc, FirstPart.get(),
    //                                             ForRangeInit.ColonLoc,
    //                                             ForRangeInit.RangeExpr.get(),
    //                                             T.getCloseLocation(),
    //                                             Sema::BFRK_Build);


  // Similarly, we need to do the semantic analysis for a for-range
  // statement immediately in order to close over temporaries correctly.
  } else if (ForEach) {
    //Diag(ForallLoc, diag::err_kitsune_for_foreach_loop_not_supported);
    // ForEachStmt = Actions.ActOnObjCForCollectionStmt(ForallLoc,
    //                                                  FirstPart.get(),
    //                                                  Collection.get(),
    //                                                  T.getCloseLocation());
  }
  // else {
  //   // In OpenMP loop region loop control variable must be captured and be
  //   // private. Perform analysis of first part (if any).
  //   if (getLangOpts().OpenMP && FirstPart.isUsable()) {
  //     Actions.ActOnOpenMPLoopInitialization(ForallLoc, FirstPart.get());
  //   }
  // }

  // The body of the _Cilk_for statement is a scope, even if there is no
  // compound stmt.  We only do this if the body isn't a compound statement to
  // avoid push/pop in common cases.
  //
  // C++ 6.5p2:
  // The substatement in an iteration-statement implicitly defines a local scope
  // which is entered and exited each time through the loop.
  //
  // See comments in ParseIfStatement for why we create a scope for
  // for-init-statement/condition and a new scope for substatement in C++.
  //
  ParseScope InnerScope(this, Scope::DeclScope, true, Tok.is(tok::l_brace));

  // The body of the for loop has the same local mangling number as the
  // for-init-statement.
  // It will only be incremented if the body contains other things that would
  // normally increment the mangling number (like a compound statement).
  getCurScope()->decrementMSManglingNumber();
  // if (C99orCXXorObjC)
  //   getCurScope()->decrementMSManglingNumber();

  // Read the body statement.
  StmtResult Body(ParseStatement(TrailingElseLoc));

  // Pop the body scope if needed.
  InnerScope.Exit();

  // Leave the for-scope.
  ForallScope.Exit();

  if (Body.isInvalid())
    return StmtError();

  // if (ForEach) {
  //   return Actions.FinishObjCForCollectionStmt(ForEachStmt.get(),
  //                                              Body.get());
  // }

  // if (ForRange) {
  //   return Actions.FinishCXXForRangeStmt(ForRangeStmt.get(), Body.get());
  // }

  return Actions.ActOnForallStmt(FirstPart.get(), nullptr, SecondPart.get().second,
                                 ThirdPart.get(), Body.get(),
                                 ForallLoc, T.getOpenLocation(),
                                 T.getCloseLocation());
}
