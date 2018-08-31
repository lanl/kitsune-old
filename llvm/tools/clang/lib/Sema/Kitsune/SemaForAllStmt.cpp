//===--- SemaStmt.cpp - Semantic Analysis for Statements ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements semantic analysis for statements.
//
//===----------------------------------------------------------------------===//

#include "clang/Sema/SemaInternal.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTDiagnostic.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/EvaluatedExprVisitor.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/TypeLoc.h"
#include "clang/AST/TypeOrdering.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Initialization.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Scope.h"
#include "clang/Sema/ScopeInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"

using namespace clang;
using namespace sema;


namespace {
  /// Produce a note indicating which begin/end function was implicitly called
  /// by a C++11 for-range statement. This is often not obvious from the code,
  /// nor from the diagnostics produced when analysing the implicit expressions
  /// required in a for-range statement.
  void NoteForAllRangeBeginEndFunction(Sema &SemaRef, Expr *E,
				       BeginEndFunction BEF) {
    CallExpr *CE = dyn_cast<CallExpr>(E);
    if (!CE)
      return;
    FunctionDecl *D = dyn_cast<FunctionDecl>(CE->getCalleeDecl());
    if (!D)
      return;
    SourceLocation Loc = D->getLocation();

    std::string Description;
    bool IsTemplate = false;
    if (FunctionTemplateDecl *FunTmpl = D->getPrimaryTemplate()) {
      Description = SemaRef.getTemplateArgumentBindingsText(
							    FunTmpl->getTemplateParameters(), *D->getTemplateSpecializationArgs());
      IsTemplate = true;
    }

    SemaRef.Diag(Loc, diag::note_for_range_begin_end)
      << BEF << IsTemplate << Description << E->getType();
  }

  /// Build a variable declaration for a for-range statement.
  VarDecl *BuildForAllRangeVarDecl(Sema &SemaRef, SourceLocation Loc,
				   QualType Type, const char *Name) {
    DeclContext *DC = SemaRef.CurContext;
    IdentifierInfo *II = &SemaRef.PP.getIdentifierTable().get(Name);
    TypeSourceInfo *TInfo = SemaRef.Context.getTrivialTypeSourceInfo(Type, Loc);
    VarDecl *Decl = VarDecl::Create(SemaRef.Context, DC, Loc, Loc, II, Type,
				    TInfo, SC_None);
    Decl->setImplicit();
    return Decl;
  }

  void CheckForAllLoopConditionalStatement(Sema &S, Expr *Second,
					   Expr *Third, Stmt *Body) {
    // Condition is empty
    if (!Second) return;

    if (S.Diags.isIgnored(diag::warn_variables_not_in_loop_body,
                          Second->getLocStart()))
      return;

    PartialDiagnostic PDiag = S.PDiag(diag::warn_variables_not_in_loop_body);
    DeclSetVector Decls;
    SmallVector<SourceRange, 10> Ranges;
    DeclExtractor DE(S, Decls, Ranges);
    DE.Visit(Second);

    // Don't analyze complex conditionals.
    if (!DE.isSimple()) return;

    // No decls found.
    if (Decls.size() == 0) return;

    // Don't warn on volatile, static, or global variables.
    for (auto *VD : Decls)
      if (VD->getType().isVolatileQualified() || VD->hasGlobalStorage())
        return;

    if (DeclMatcher(S, Decls, Second).FoundDeclInUse() ||
        DeclMatcher(S, Decls, Third).FoundDeclInUse() ||
        DeclMatcher(S, Decls, Body).FoundDeclInUse())
      return;

    // Load decl names into diagnostic.
    if (Decls.size() > 4) {
      PDiag << 0;
    } else {
      PDiag << (unsigned)Decls.size();
      for (auto *VD : Decls)
        PDiag << VD->getDeclName();
    }

    for (auto Range : Ranges)
      PDiag << Range;

    S.Diag(Ranges.begin()->getBegin(), PDiag);
  }

  
}


StmtResult Sema::ActOnForAllStmt(SourceLocation ForLoc, SourceLocation LParenLoc,
				 Stmt *First, ConditionResult Second,
				 FullExprArg third, SourceLocation RParenLoc,
				 Stmt *Body) {
  if (Second.isInvalid())
    return StmtError();

  if (!getLangOpts().CPlusPlus) {
    if (DeclStmt *DS = dyn_cast_or_null<DeclStmt>(First)) {
      // C99 6.8.5p3: The declaration part of a 'for' statement shall only
      // declare identifiers for objects having storage class 'auto' or
      // 'register'.
      for (auto *DI : DS->decls()) {
        VarDecl *VD = dyn_cast<VarDecl>(DI);
        if (VD && VD->isLocalVarDecl() && !VD->hasLocalStorage())
          VD = nullptr;
        if (!VD) {
          Diag(DI->getLocation(), diag::err_non_local_variable_decl_in_for);
          DI->setInvalidDecl();
        }
      }
    }
  }

  CheckBreakContinueBinding(Second.get().second);
  CheckBreakContinueBinding(third.get());

  if (!Second.get().first)
    CheckForAllLoopConditionalStatement(*this, Second.get().second, third.get(), Body);
  CheckForRedundantIteration(*this, third.get(), Body);

  if (Second.get().second &&
      !Diags.isIgnored(diag::warn_comma_operator,
                       Second.get().second->getExprLoc()))
    CommaVisitor(*this).Visit(Second.get().second);

  Expr *Third  = third.release().getAs<Expr>();

  DiagnoseUnusedExprResult(First);
  DiagnoseUnusedExprResult(Third);
  DiagnoseUnusedExprResult(Body);

  if (isa<NullStmt>(Body))
    getCurCompoundScope().setHasEmptyLoopBodies();

  return new (Context)
    ForAllStmt(Context, First, Second.get().second, Second.get().first, Third,
	       Body, ForLoc, LParenLoc, RParenLoc);
}

/// Finish building a variable declaration for a for-range statement.
/// \return true if an error occurs.
static bool FinishForAllRangeVarDecl(Sema &SemaRef, VarDecl *Decl, Expr *Init,
				     SourceLocation Loc, int DiagID) {
  if (Decl->getType()->isUndeducedType()) {
    ExprResult Res = SemaRef.CorrectDelayedTyposInExpr(Init);
    if (!Res.isUsable()) {
      Decl->setInvalidDecl();
      return true;
    }
    Init = Res.get();
  }

  // Deduce the type for the iterator variable now rather than leaving it to
  // AddInitializerToDecl, so we can produce a more suitable diagnostic.
  QualType InitType;
  if ((!isa<InitListExpr>(Init) && Init->getType()->isVoidType()) ||
      SemaRef.DeduceAutoType(Decl->getTypeSourceInfo(), Init, InitType) ==
          Sema::DAR_Failed)
    SemaRef.Diag(Loc, DiagID) << Init->getType();
  if (InitType.isNull()) {
    Decl->setInvalidDecl();
    return true;
  }
  Decl->setType(InitType);

  SemaRef.AddInitializerToDecl(Decl, Init, /*DirectInit=*/false);
  SemaRef.FinalizeDeclaration(Decl);
  SemaRef.CurContext->addHiddenDecl(Decl);
  return false;
}

/// ActOnCXXForAllRangeStmt - Check and build a C++11 for-range statement.
///
/// C++11 [stmt.ranged]:
///   A range-based forall statement is equivalent to
///
///   {
///     auto && __range = range-init;
///     forall ( auto __begin = begin-expr,
///           __end = end-expr;
///           __begin != __end;
///           ++__begin ) {
///       for-range-declaration = *__begin;
///       statement
///     }
///   }
///
/// The body of the loop is not available yet, since it cannot be analysed until
/// we have determined the type of the for-range-declaration.
StmtResult Sema::ActOnCXXForAllRangeStmt(Scope *S, SourceLocation ForLoc,
					 SourceLocation CoawaitLoc, Stmt *First,
					 SourceLocation ColonLoc, Expr *Range,
					 SourceLocation RParenLoc,
					 BuildForRangeKind Kind) {
  if (!First)
    return StmtError();

  if (Range && ObjCEnumerationCollection(Range))
    return ActOnObjCForCollectionStmt(ForLoc, First, Range, RParenLoc);

  DeclStmt *DS = dyn_cast<DeclStmt>(First);
  assert(DS && "first part of for range not a decl stmt");

  if (!DS->isSingleDecl()) {
    Diag(DS->getStartLoc(), diag::err_type_defined_in_for_range);
    return StmtError();
  }

  Decl *LoopVar = DS->getSingleDecl();
  if (LoopVar->isInvalidDecl() || !Range ||
      DiagnoseUnexpandedParameterPack(Range, UPPC_Expression)) {
    LoopVar->setInvalidDecl();
    return StmtError();
  }

  // Build the coroutine state immediately and not later during template
  // instantiation
  if (!CoawaitLoc.isInvalid()) {
    if (!ActOnCoroutineBodyStart(S, CoawaitLoc, "co_await"))
      return StmtError();
  }

  // Build  auto && __range = range-init
  SourceLocation RangeLoc = Range->getLocStart();
  VarDecl *RangeVar = BuildForAllRangeVarDecl(*this, RangeLoc,
                                           Context.getAutoRRefDeductType(),
                                           "__range");
  if (FinishForAllRangeVarDecl(*this, RangeVar, Range, RangeLoc,
                            diag::err_for_range_deduction_failure)) {
    LoopVar->setInvalidDecl();
    return StmtError();
  }

  // Claim the type doesn't contain auto: we've already done the checking.
  DeclGroupPtrTy RangeGroup =
      BuildDeclaratorGroup(MutableArrayRef<Decl *>((Decl **)&RangeVar, 1));
  StmtResult RangeDecl = ActOnDeclStmt(RangeGroup, RangeLoc, RangeLoc);
  if (RangeDecl.isInvalid()) {
    LoopVar->setInvalidDecl();
    return StmtError();
  }

  return BuildCXXForAllRangeStmt(ForLoc, CoawaitLoc, ColonLoc, RangeDecl.get(),
				 /*BeginStmt=*/nullptr, /*EndStmt=*/nullptr,
				 /*Cond=*/nullptr, /*Inc=*/nullptr,
				 DS, RParenLoc, Kind);
}

/// \brief Create the initialization, compare, and increment steps for
/// the range-based for loop expression.
/// This function does not handle array-based for loops,
/// which are created in Sema::BuildCXXForAllRangeStmt.
///
/// \returns a ForRangeStatus indicating success or what kind of error occurred.
/// BeginExpr and EndExpr are set and FRS_Success is returned on success;
/// CandidateSet and BEF are set and some non-success value is returned on
/// failure.
static Sema::ForRangeStatus
BuildNonArrayForAllRange(Sema &SemaRef, Expr *BeginRange, Expr *EndRange,
			 QualType RangeType, VarDecl *BeginVar, VarDecl *EndVar,
			 SourceLocation ColonLoc, SourceLocation CoawaitLoc,
			 OverloadCandidateSet *CandidateSet, ExprResult *BeginExpr,
			 ExprResult *EndExpr, BeginEndFunction *BEF) {
  DeclarationNameInfo BeginNameInfo(
      &SemaRef.PP.getIdentifierTable().get("begin"), ColonLoc);
  DeclarationNameInfo EndNameInfo(&SemaRef.PP.getIdentifierTable().get("end"),
                                  ColonLoc);

  LookupResult BeginMemberLookup(SemaRef, BeginNameInfo,
                                 Sema::LookupMemberName);
  LookupResult EndMemberLookup(SemaRef, EndNameInfo, Sema::LookupMemberName);

  if (CXXRecordDecl *D = RangeType->getAsCXXRecordDecl()) {
    // - if _RangeT is a class type, the unqualified-ids begin and end are
    //   looked up in the scope of class _RangeT as if by class member access
    //   lookup (3.4.5), and if either (or both) finds at least one
    //   declaration, begin-expr and end-expr are __range.begin() and
    //   __range.end(), respectively;
    SemaRef.LookupQualifiedName(BeginMemberLookup, D);
    SemaRef.LookupQualifiedName(EndMemberLookup, D);

    if (BeginMemberLookup.empty() != EndMemberLookup.empty()) {
      SourceLocation RangeLoc = BeginVar->getLocation();
      *BEF = BeginMemberLookup.empty() ? BEF_end : BEF_begin;

      SemaRef.Diag(RangeLoc, diag::err_for_range_member_begin_end_mismatch)
          << RangeLoc << BeginRange->getType() << *BEF;
      return Sema::FRS_DiagnosticIssued;
    }
  } else {
    // - otherwise, begin-expr and end-expr are begin(__range) and
    //   end(__range), respectively, where begin and end are looked up with
    //   argument-dependent lookup (3.4.2). For the purposes of this name
    //   lookup, namespace std is an associated namespace.

  }

  *BEF = BEF_begin;
  Sema::ForRangeStatus RangeStatus =
      SemaRef.BuildForAllRangeBeginEndCall(ColonLoc, ColonLoc, BeginNameInfo,
                                        BeginMemberLookup, CandidateSet,
                                        BeginRange, BeginExpr);

  if (RangeStatus != Sema::FRS_Success) {
    if (RangeStatus == Sema::FRS_DiagnosticIssued)
      SemaRef.Diag(BeginRange->getLocStart(), diag::note_in_for_range)
          << ColonLoc << BEF_begin << BeginRange->getType();
    return RangeStatus;
  }
  if (!CoawaitLoc.isInvalid()) {
    // FIXME: getCurScope() should not be used during template instantiation.
    // We should pick up the set of unqualified lookup results for operator
    // co_await during the initial parse.
    *BeginExpr = SemaRef.ActOnCoawaitExpr(SemaRef.getCurScope(), ColonLoc,
                                          BeginExpr->get());
    if (BeginExpr->isInvalid())
      return Sema::FRS_DiagnosticIssued;
  }
  if (FinishForAllRangeVarDecl(SemaRef, BeginVar, BeginExpr->get(), ColonLoc,
                            diag::err_for_range_iter_deduction_failure)) {
    NoteForAllRangeBeginEndFunction(SemaRef, BeginExpr->get(), *BEF);
    return Sema::FRS_DiagnosticIssued;
  }

  *BEF = BEF_end;
  RangeStatus =
      SemaRef.BuildForAllRangeBeginEndCall(ColonLoc, ColonLoc, EndNameInfo,
					   EndMemberLookup, CandidateSet,
					   EndRange, EndExpr);
  if (RangeStatus != Sema::FRS_Success) {
    if (RangeStatus == Sema::FRS_DiagnosticIssued)
      SemaRef.Diag(EndRange->getLocStart(), diag::note_in_for_range)
          << ColonLoc << BEF_end << EndRange->getType();
    return RangeStatus;
  }
  if (FinishForAllRangeVarDecl(SemaRef, EndVar, EndExpr->get(), ColonLoc,
                            diag::err_for_range_iter_deduction_failure)) {
    NoteForAllRangeBeginEndFunction(SemaRef, EndExpr->get(), *BEF);
    return Sema::FRS_DiagnosticIssued;
  }
  return Sema::FRS_Success;
}

/// Speculatively attempt to dereference an invalid range expression.
/// If the attempt fails, this function will return a valid, null StmtResult
/// and emit no diagnostics.
static StmtResult RebuildForAllRangeWithDereference(Sema &SemaRef, Scope *S,
						    SourceLocation ForLoc,
						    SourceLocation CoawaitLoc,
						    Stmt *LoopVarDecl,
						    SourceLocation ColonLoc,
						    Expr *Range,
						    SourceLocation RangeLoc,
						    SourceLocation RParenLoc) {
  // Determine whether we can rebuild the for-range statement with a
  // dereferenced range expression.
  ExprResult AdjustedRange;
  {
    Sema::SFINAETrap Trap(SemaRef);

    AdjustedRange = SemaRef.BuildUnaryOp(S, RangeLoc, UO_Deref, Range);
    if (AdjustedRange.isInvalid())
      return StmtResult();

    StmtResult SR = SemaRef.ActOnCXXForAllRangeStmt(
        S, ForLoc, CoawaitLoc, LoopVarDecl, ColonLoc, AdjustedRange.get(),
        RParenLoc, Sema::BFRK_Check);
    if (SR.isInvalid())
      return StmtResult();
  }

  // The attempt to dereference worked well enough that it could produce a valid
  // loop. Produce a fixit, and rebuild the loop with diagnostics enabled, in
  // case there are any other (non-fatal) problems with it.
  SemaRef.Diag(RangeLoc, diag::err_for_range_dereference)
    << Range->getType() << FixItHint::CreateInsertion(RangeLoc, "*");
  return SemaRef.ActOnCXXForAllRangeStmt(S, ForLoc, CoawaitLoc, LoopVarDecl,
					 ColonLoc, AdjustedRange.get(), RParenLoc,
					 Sema::BFRK_Rebuild);
}

/// BuildCXXForAllRangeStmt - Build or instantiate a C++11 for-range statement.
StmtResult
Sema::BuildCXXForAllRangeStmt(SourceLocation ForLoc, SourceLocation CoawaitLoc,
			      SourceLocation ColonLoc, Stmt *RangeDecl,
			      Stmt *Begin, Stmt *End, Expr *Cond,
			      Expr *Inc, Stmt *LoopVarDecl,
			      SourceLocation RParenLoc, BuildForRangeKind Kind) {
  // FIXME: This should not be used during template instantiation. We should
  // pick up the set of unqualified lookup results for the != and + operators
  // in the initial parse.
  //
  // Testcase (accepts-invalid):
  //   template<typename T> void f() { for (auto x : T()) {} }
  //   namespace N { struct X { X begin(); X end(); int operator*(); }; }
  //   bool operator!=(N::X, N::X); void operator++(N::X);
  //   void g() { f<N::X>(); }
  Scope *S = getCurScope();

  DeclStmt *RangeDS = cast<DeclStmt>(RangeDecl);
  VarDecl *RangeVar = cast<VarDecl>(RangeDS->getSingleDecl());
  QualType RangeVarType = RangeVar->getType();

  DeclStmt *LoopVarDS = cast<DeclStmt>(LoopVarDecl);
  VarDecl *LoopVar = cast<VarDecl>(LoopVarDS->getSingleDecl());

  // If we hit any errors, mark the loop variable as invalid if its type
  // contains 'auto'.
  InvalidateOnErrorScope Invalidate(*this, LoopVar,
                                    LoopVar->getType()->isUndeducedType());

  StmtResult BeginDeclStmt = Begin;
  StmtResult EndDeclStmt = End;
  ExprResult NotEqExpr = Cond, IncrExpr = Inc;

  if (RangeVarType->isDependentType()) {
    // The range is implicitly used as a placeholder when it is dependent.
    RangeVar->markUsed(Context);

    // Deduce any 'auto's in the loop variable as 'DependentTy'. We'll fill
    // them in properly when we instantiate the loop.
    if (!LoopVar->isInvalidDecl() && Kind != BFRK_Check) {
      if (auto *DD = dyn_cast<DecompositionDecl>(LoopVar))
        for (auto *Binding : DD->bindings())
          Binding->setType(Context.DependentTy);
      LoopVar->setType(SubstAutoType(LoopVar->getType(), Context.DependentTy));
    }
  } else if (!BeginDeclStmt.get()) {
    SourceLocation RangeLoc = RangeVar->getLocation();

    const QualType RangeVarNonRefType = RangeVarType.getNonReferenceType();

    ExprResult BeginRangeRef = BuildDeclRefExpr(RangeVar, RangeVarNonRefType,
                                                VK_LValue, ColonLoc);
    if (BeginRangeRef.isInvalid())
      return StmtError();

    ExprResult EndRangeRef = BuildDeclRefExpr(RangeVar, RangeVarNonRefType,
                                              VK_LValue, ColonLoc);
    if (EndRangeRef.isInvalid())
      return StmtError();

    QualType AutoType = Context.getAutoDeductType();
    Expr *Range = RangeVar->getInit();
    if (!Range)
      return StmtError();
    QualType RangeType = Range->getType();

    if (RequireCompleteType(RangeLoc, RangeType,
                            diag::err_for_range_incomplete_type))
      return StmtError();

    // Build auto __begin = begin-expr, __end = end-expr.
    VarDecl *BeginVar = BuildForAllRangeVarDecl(*this, ColonLoc, AutoType,
                                             "__begin");
    VarDecl *EndVar = BuildForAllRangeVarDecl(*this, ColonLoc, AutoType,
                                           "__end");

    // Build begin-expr and end-expr and attach to __begin and __end variables.
    ExprResult BeginExpr, EndExpr;
    if (const ArrayType *UnqAT = RangeType->getAsArrayTypeUnsafe()) {
      // - if _RangeT is an array type, begin-expr and end-expr are __range and
      //   __range + __bound, respectively, where __bound is the array bound. If
      //   _RangeT is an array of unknown size or an array of incomplete type,
      //   the program is ill-formed;

      // begin-expr is __range.
      BeginExpr = BeginRangeRef;
      if (!CoawaitLoc.isInvalid()) {
        BeginExpr = ActOnCoawaitExpr(S, ColonLoc, BeginExpr.get());
        if (BeginExpr.isInvalid())
          return StmtError();
      }
      if (FinishForAllRangeVarDecl(*this, BeginVar, BeginRangeRef.get(), ColonLoc,
                                diag::err_for_range_iter_deduction_failure)) {
        NoteForAllRangeBeginEndFunction(*this, BeginExpr.get(), BEF_begin);
        return StmtError();
      }

      // Find the array bound.
      ExprResult BoundExpr;
      if (const ConstantArrayType *CAT = dyn_cast<ConstantArrayType>(UnqAT))
        BoundExpr = IntegerLiteral::Create(
            Context, CAT->getSize(), Context.getPointerDiffType(), RangeLoc);
      else if (const VariableArrayType *VAT =
               dyn_cast<VariableArrayType>(UnqAT)) {
        // For a variably modified type we can't just use the expression within
        // the array bounds, since we don't want that to be re-evaluated here.
        // Rather, we need to determine what it was when the array was first
        // created - so we resort to using sizeof(vla)/sizeof(element).
        // For e.g.
        //  void f(int b) { 
        //    int vla[b];
        //    b = -1;   <-- This should not affect the num of iterations below
        //    for (int &c : vla) { .. }
        //  }

        // FIXME: This results in codegen generating IR that recalculates the
        // run-time number of elements (as opposed to just using the IR Value
        // that corresponds to the run-time value of each bound that was
        // generated when the array was created.) If this proves too embarassing
        // even for unoptimized IR, consider passing a magic-value/cookie to
        // codegen that then knows to simply use that initial llvm::Value (that
        // corresponds to the bound at time of array creation) within
        // getelementptr.  But be prepared to pay the price of increasing a
        // customized form of coupling between the two components - which  could
        // be hard to maintain as the codebase evolves.

        ExprResult SizeOfVLAExprR = ActOnUnaryExprOrTypeTraitExpr(
            EndVar->getLocation(), UETT_SizeOf,
            /*isType=*/true,
            CreateParsedType(VAT->desugar(), Context.getTrivialTypeSourceInfo(
                                                 VAT->desugar(), RangeLoc))
                .getAsOpaquePtr(),
            EndVar->getSourceRange());
        if (SizeOfVLAExprR.isInvalid())
          return StmtError();
        
        ExprResult SizeOfEachElementExprR = ActOnUnaryExprOrTypeTraitExpr(
            EndVar->getLocation(), UETT_SizeOf,
            /*isType=*/true,
            CreateParsedType(VAT->desugar(),
                             Context.getTrivialTypeSourceInfo(
                                 VAT->getElementType(), RangeLoc))
                .getAsOpaquePtr(),
            EndVar->getSourceRange());
        if (SizeOfEachElementExprR.isInvalid())
          return StmtError();

        BoundExpr =
            ActOnBinOp(S, EndVar->getLocation(), tok::slash,
                       SizeOfVLAExprR.get(), SizeOfEachElementExprR.get());
        if (BoundExpr.isInvalid())
          return StmtError();
        
      } else {
        // Can't be a DependentSizedArrayType or an IncompleteArrayType since
        // UnqAT is not incomplete and Range is not type-dependent.
        llvm_unreachable("Unexpected array type in for-range");
      }

      // end-expr is __range + __bound.
      EndExpr = ActOnBinOp(S, ColonLoc, tok::plus, EndRangeRef.get(),
                           BoundExpr.get());
      if (EndExpr.isInvalid())
        return StmtError();
      if (FinishForAllRangeVarDecl(*this, EndVar, EndExpr.get(), ColonLoc,
                                diag::err_for_range_iter_deduction_failure)) {
        NoteForAllRangeBeginEndFunction(*this, EndExpr.get(), BEF_end);
        return StmtError();
      }
    } else {
      OverloadCandidateSet CandidateSet(RangeLoc,
                                        OverloadCandidateSet::CSK_Normal);
      BeginEndFunction BEFFailure;
      ForRangeStatus RangeStatus = BuildNonArrayForAllRange(
          *this, BeginRangeRef.get(), EndRangeRef.get(), RangeType, BeginVar,
          EndVar, ColonLoc, CoawaitLoc, &CandidateSet, &BeginExpr, &EndExpr,
          &BEFFailure);

      if (Kind == BFRK_Build && RangeStatus == FRS_NoViableFunction &&
          BEFFailure == BEF_begin) {
        // If the range is being built from an array parameter, emit a
        // a diagnostic that it is being treated as a pointer.
        if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(Range)) {
          if (ParmVarDecl *PVD = dyn_cast<ParmVarDecl>(DRE->getDecl())) {
            QualType ArrayTy = PVD->getOriginalType();
            QualType PointerTy = PVD->getType();
            if (PointerTy->isPointerType() && ArrayTy->isArrayType()) {
              Diag(Range->getLocStart(), diag::err_range_on_array_parameter)
                << RangeLoc << PVD << ArrayTy << PointerTy;
              Diag(PVD->getLocation(), diag::note_declared_at);
              return StmtError();
            }
          }
        }

        // If building the range failed, try dereferencing the range expression
        // unless a diagnostic was issued or the end function is problematic.
        StmtResult SR = RebuildForAllRangeWithDereference(*this, S, ForLoc,
                                                       CoawaitLoc,
                                                       LoopVarDecl, ColonLoc,
                                                       Range, RangeLoc,
                                                       RParenLoc);
        if (SR.isInvalid() || SR.isUsable())
          return SR;
      }

      // Otherwise, emit diagnostics if we haven't already.
      if (RangeStatus == FRS_NoViableFunction) {
        Expr *Range = BEFFailure ? EndRangeRef.get() : BeginRangeRef.get();
        Diag(Range->getLocStart(), diag::err_for_range_invalid)
            << RangeLoc << Range->getType() << BEFFailure;
        CandidateSet.NoteCandidates(*this, OCD_AllCandidates, Range);
      }
      // Return an error if no fix was discovered.
      if (RangeStatus != FRS_Success)
        return StmtError();
    }

    assert(!BeginExpr.isInvalid() && !EndExpr.isInvalid() &&
           "invalid range expression in for loop");

    // C++11 [dcl.spec.auto]p7: BeginType and EndType must be the same.
    // C++1z removes this restriction.
    QualType BeginType = BeginVar->getType(), EndType = EndVar->getType();
    if (!Context.hasSameType(BeginType, EndType)) {
      Diag(RangeLoc, getLangOpts().CPlusPlus1z
                         ? diag::warn_for_range_begin_end_types_differ
                         : diag::ext_for_range_begin_end_types_differ)
          << BeginType << EndType;
      NoteForAllRangeBeginEndFunction(*this, BeginExpr.get(), BEF_begin);
      NoteForAllRangeBeginEndFunction(*this, EndExpr.get(), BEF_end);
    }

    BeginDeclStmt =
        ActOnDeclStmt(ConvertDeclToDeclGroup(BeginVar), ColonLoc, ColonLoc);
    EndDeclStmt =
        ActOnDeclStmt(ConvertDeclToDeclGroup(EndVar), ColonLoc, ColonLoc);

    const QualType BeginRefNonRefType = BeginType.getNonReferenceType();
    ExprResult BeginRef = BuildDeclRefExpr(BeginVar, BeginRefNonRefType,
                                           VK_LValue, ColonLoc);
    if (BeginRef.isInvalid())
      return StmtError();

    ExprResult EndRef = BuildDeclRefExpr(EndVar, EndType.getNonReferenceType(),
                                         VK_LValue, ColonLoc);
    if (EndRef.isInvalid())
      return StmtError();

    // Build and check __begin != __end expression.
    NotEqExpr = ActOnBinOp(S, ColonLoc, tok::exclaimequal,
                           BeginRef.get(), EndRef.get());
    if (!NotEqExpr.isInvalid())
      NotEqExpr = CheckBooleanCondition(ColonLoc, NotEqExpr.get());
    if (!NotEqExpr.isInvalid())
      NotEqExpr = ActOnFinishFullExpr(NotEqExpr.get());
    if (NotEqExpr.isInvalid()) {
      Diag(RangeLoc, diag::note_for_range_invalid_iterator)
        << RangeLoc << 0 << BeginRangeRef.get()->getType();
      NoteForAllRangeBeginEndFunction(*this, BeginExpr.get(), BEF_begin);
      if (!Context.hasSameType(BeginType, EndType))
        NoteForAllRangeBeginEndFunction(*this, EndExpr.get(), BEF_end);
      return StmtError();
    }

    // Build and check ++__begin expression.
    BeginRef = BuildDeclRefExpr(BeginVar, BeginRefNonRefType,
                                VK_LValue, ColonLoc);
    if (BeginRef.isInvalid())
      return StmtError();

    IncrExpr = ActOnUnaryOp(S, ColonLoc, tok::plusplus, BeginRef.get());
    if (!IncrExpr.isInvalid() && CoawaitLoc.isValid())
      // FIXME: getCurScope() should not be used during template instantiation.
      // We should pick up the set of unqualified lookup results for operator
      // co_await during the initial parse.
      IncrExpr = ActOnCoawaitExpr(S, CoawaitLoc, IncrExpr.get());
    if (!IncrExpr.isInvalid())
      IncrExpr = ActOnFinishFullExpr(IncrExpr.get());
    if (IncrExpr.isInvalid()) {
      Diag(RangeLoc, diag::note_for_range_invalid_iterator)
        << RangeLoc << 2 << BeginRangeRef.get()->getType() ;
      NoteForAllRangeBeginEndFunction(*this, BeginExpr.get(), BEF_begin);
      return StmtError();
    }

    // Build and check *__begin  expression.
    BeginRef = BuildDeclRefExpr(BeginVar, BeginRefNonRefType,
                                VK_LValue, ColonLoc);
    if (BeginRef.isInvalid())
      return StmtError();

    ExprResult DerefExpr = ActOnUnaryOp(S, ColonLoc, tok::star, BeginRef.get());
    if (DerefExpr.isInvalid()) {
      Diag(RangeLoc, diag::note_for_range_invalid_iterator)
        << RangeLoc << 1 << BeginRangeRef.get()->getType();
      NoteForAllRangeBeginEndFunction(*this, BeginExpr.get(), BEF_begin);
      return StmtError();
    }

    // Attach  *__begin  as initializer for VD. Don't touch it if we're just
    // trying to determine whether this would be a valid range.
    if (!LoopVar->isInvalidDecl() && Kind != BFRK_Check) {
      AddInitializerToDecl(LoopVar, DerefExpr.get(), /*DirectInit=*/false);
      if (LoopVar->isInvalidDecl())
        NoteForAllRangeBeginEndFunction(*this, BeginExpr.get(), BEF_begin);
    }
  }

  // Don't bother to actually allocate the result if we're just trying to
  // determine whether it would be valid.
  if (Kind == BFRK_Check)
    return StmtResult();

  return new (Context) CXXForAllRangeStmt(
      RangeDS, cast_or_null<DeclStmt>(BeginDeclStmt.get()),
      cast_or_null<DeclStmt>(EndDeclStmt.get()), NotEqExpr.get(),
      IncrExpr.get(), LoopVarDS, /*Body=*/nullptr, ForLoc, CoawaitLoc,
      ColonLoc, RParenLoc);
}

/// FinishObjCForCollectionStmt - Attach the body to a objective-C foreach
/// statement.
StmtResult Sema::FinishObjCForCollectionStmt(Stmt *S, Stmt *B) {
  if (!S || !B)
    return StmtError();
  ObjCForCollectionStmt * ForStmt = cast<ObjCForCollectionStmt>(S);

  ForStmt->setBody(B);
  return S;
}

// Warn when the loop variable is a const reference that creates a copy.
// Suggest using the non-reference type for copies.  If a copy can be prevented
// suggest the const reference type that would do so.
// For instance, given "for (const &Foo : Range)", suggest
// "for (const Foo : Range)" to denote a copy is made for the loop.  If
// possible, also suggest "for (const &Bar : Range)" if this type prevents
// the copy altogether.
static void DiagnoseForAllRangeReferenceVariableCopies(Sema &SemaRef,
                                                    const VarDecl *VD,
                                                    QualType RangeInitType) {
  const Expr *InitExpr = VD->getInit();
  if (!InitExpr)
    return;

  QualType VariableType = VD->getType();

  if (auto Cleanups = dyn_cast<ExprWithCleanups>(InitExpr))
    if (!Cleanups->cleanupsHaveSideEffects())
      InitExpr = Cleanups->getSubExpr();

  const MaterializeTemporaryExpr *MTE =
      dyn_cast<MaterializeTemporaryExpr>(InitExpr);

  // No copy made.
  if (!MTE)
    return;

  const Expr *E = MTE->GetTemporaryExpr()->IgnoreImpCasts();

  // Searching for either UnaryOperator for dereference of a pointer or
  // CXXOperatorCallExpr for handling iterators.
  while (!isa<CXXOperatorCallExpr>(E) && !isa<UnaryOperator>(E)) {
    if (const CXXConstructExpr *CCE = dyn_cast<CXXConstructExpr>(E)) {
      E = CCE->getArg(0);
    } else if (const CXXMemberCallExpr *Call = dyn_cast<CXXMemberCallExpr>(E)) {
      const MemberExpr *ME = cast<MemberExpr>(Call->getCallee());
      E = ME->getBase();
    } else {
      const MaterializeTemporaryExpr *MTE = cast<MaterializeTemporaryExpr>(E);
      E = MTE->GetTemporaryExpr();
    }
    E = E->IgnoreImpCasts();
  }

  bool ReturnsReference = false;
  if (isa<UnaryOperator>(E)) {
    ReturnsReference = true;
  } else {
    const CXXOperatorCallExpr *Call = cast<CXXOperatorCallExpr>(E);
    const FunctionDecl *FD = Call->getDirectCallee();
    QualType ReturnType = FD->getReturnType();
    ReturnsReference = ReturnType->isReferenceType();
  }

  if (ReturnsReference) {
    // Loop variable creates a temporary.  Suggest either to go with
    // non-reference loop variable to indiciate a copy is made, or
    // the correct time to bind a const reference.
    SemaRef.Diag(VD->getLocation(), diag::warn_for_range_const_reference_copy)
        << VD << VariableType << E->getType();
    QualType NonReferenceType = VariableType.getNonReferenceType();
    NonReferenceType.removeLocalConst();
    QualType NewReferenceType =
        SemaRef.Context.getLValueReferenceType(E->getType().withConst());
    SemaRef.Diag(VD->getLocStart(), diag::note_use_type_or_non_reference)
        << NonReferenceType << NewReferenceType << VD->getSourceRange();
  } else {
    // The range always returns a copy, so a temporary is always created.
    // Suggest removing the reference from the loop variable.
    SemaRef.Diag(VD->getLocation(), diag::warn_for_range_variable_always_copy)
        << VD << RangeInitType;
    QualType NonReferenceType = VariableType.getNonReferenceType();
    NonReferenceType.removeLocalConst();
    SemaRef.Diag(VD->getLocStart(), diag::note_use_non_reference_type)
        << NonReferenceType << VD->getSourceRange();
  }
}

// Warns when the loop variable can be changed to a reference type to
// prevent a copy.  For instance, if given "for (const Foo x : Range)" suggest
// "for (const Foo &x : Range)" if this form does not make a copy.
static void DiagnoseForAllRangeConstVariableCopies(Sema &SemaRef,
                                                const VarDecl *VD) {
  const Expr *InitExpr = VD->getInit();
  if (!InitExpr)
    return;

  QualType VariableType = VD->getType();

  if (const CXXConstructExpr *CE = dyn_cast<CXXConstructExpr>(InitExpr)) {
    if (!CE->getConstructor()->isCopyConstructor())
      return;
  } else if (const CastExpr *CE = dyn_cast<CastExpr>(InitExpr)) {
    if (CE->getCastKind() != CK_LValueToRValue)
      return;
  } else {
    return;
  }

  // TODO: Determine a maximum size that a POD type can be before a diagnostic
  // should be emitted.  Also, only ignore POD types with trivial copy
  // constructors.
  if (VariableType.isPODType(SemaRef.Context))
    return;

  // Suggest changing from a const variable to a const reference variable
  // if doing so will prevent a copy.
  SemaRef.Diag(VD->getLocation(), diag::warn_for_range_copy)
      << VD << VariableType << InitExpr->getType();
  SemaRef.Diag(VD->getLocStart(), diag::note_use_reference_type)
      << SemaRef.Context.getLValueReferenceType(VariableType)
      << VD->getSourceRange();
}

/// DiagnoseForAllRangeVariableCopies - Diagnose three cases and fixes for them.
/// 1) for (const foo &x : foos) where foos only returns a copy.  Suggest
///    using "const foo x" to show that a copy is made
/// 2) for (const bar &x : foos) where bar is a temporary intialized by bar.
///    Suggest either "const bar x" to keep the copying or "const foo& x" to
///    prevent the copy.
/// 3) for (const foo x : foos) where x is constructed from a reference foo.
///    Suggest "const foo &x" to prevent the copy.
static void DiagnoseForAllRangeVariableCopies(Sema &SemaRef,
                                           const CXXForAllRangeStmt *ForStmt) {
  if (SemaRef.Diags.isIgnored(diag::warn_for_range_const_reference_copy,
                              ForStmt->getLocStart()) &&
      SemaRef.Diags.isIgnored(diag::warn_for_range_variable_always_copy,
                              ForStmt->getLocStart()) &&
      SemaRef.Diags.isIgnored(diag::warn_for_range_copy,
                              ForStmt->getLocStart())) {
    return;
  }

  const VarDecl *VD = ForStmt->getLoopVariable();
  if (!VD)
    return;

  QualType VariableType = VD->getType();

  if (VariableType->isIncompleteType())
    return;

  const Expr *InitExpr = VD->getInit();
  if (!InitExpr)
    return;

  if (VariableType->isReferenceType()) {
    DiagnoseForAllRangeReferenceVariableCopies(SemaRef, VD,
                                            ForStmt->getRangeInit()->getType());
  } else if (VariableType.isConstQualified()) {
    DiagnoseForAllRangeConstVariableCopies(SemaRef, VD);
  }
}

/// FinishCXXForAllRangeStmt - Attach the body to a C++0x for-range statement.
/// This is a separate step from ActOnCXXForAllRangeStmt because analysis of the
/// body cannot be performed until after the type of the range variable is
/// determined.
StmtResult Sema::FinishCXXForAllRangeStmt(Stmt *S, Stmt *B) {
  if (!S || !B)
    return StmtError();

  CXXForAllRangeStmt *ForStmt = cast<CXXForAllRangeStmt>(S);
  ForStmt->setBody(B);

  DiagnoseEmptyStmtBody(ForStmt->getRParenLoc(), B,
                        diag::warn_empty_range_based_for_body);

  DiagnoseForAllRangeVariableCopies(*this, ForStmt);

  return S;
}
