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

#ifndef KITSUNE_LLVM_CLANG_AST_STMT_H
#define KITSUNE_LLVM_CLANG_AST_STMT_H

#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"


namespace clang {

class ForAllStmt : public Stmt {
  SourceLocation ForLoc;
  enum { INIT, CONDVAR, COND, INC, BODY, END_EXPR };
  Stmt* SubExprs[END_EXPR]; // SubExprs[INIT] is an expression or declstmt.
  SourceLocation LParenLoc, RParenLoc;

public:
  ForAllStmt ASTContext &C, Stmt *Init, Expr *Cond, VarDecl *condVar,
    Expr *Inc, Stmt *Body, SourceLocation FL, SourceLocation LP,
    SourceLocation RP);

  /// \brief Build an empty for statement.
  explicit ForAllStmt(EmptyShell Empty) : Stmt(ForStmtClass, Empty) { }

  Stmt *getInit() { return SubExprs[INIT]; }

  /// \brief Retrieve the variable declared in this "for" statement, if any.
  ///
  /// In the following example, "y" is the condition variable.
  /// \code
  /// for (int x = random(); int y = mangle(x); ++x) {
  ///   // ...
  /// }
  /// \endcode
  VarDecl *getConditionVariable() const;
  void setConditionVariable(const ASTContext &C, VarDecl *V);

  /// If this ForStmt has a condition variable, return the faux DeclStmt
  /// associated with the creation of that condition variable.
  const DeclStmt *getConditionVariableDeclStmt() const {
    return reinterpret_cast<DeclStmt*>(SubExprs[CONDVAR]);
  }

  };

  
// +===== Kitsune

class KitsuneStmt : public Stmt {
public:
  enum KitsuneStmtKind{
    Forall
  };

  KitsuneStmt(KitsuneStmtKind K)
  : Stmt(KitsuneStmtClass),
    Kind(K){}

  virtual ~KitsuneStmt(){}

  /// \brief Build an empty for statement.
  explicit KitsuneStmt(EmptyShell Empty) : Stmt(KitsuneStmtClass, Empty) { }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == KitsuneStmtClass;
  }

  KitsuneStmtKind kind() const{
    return Kind;
  }
  
  static bool classof(const KitsuneStmt *){ return true; }
  
  virtual child_range children(){
    return child_range(child_iterator(), child_iterator());
  }

  SourceLocation getLocStart() const LLVM_READONLY{
    return StartLoc;
  }
  
  void setLocStart(SourceLocation Loc){
    StartLoc = Loc;
  }
  
  SourceLocation getLocEnd() const LLVM_READONLY{
    return EndLoc;
  }
  
  void setLocEnd(SourceLocation Loc){
    EndLoc = Loc;
  }

private:
  KitsuneStmtKind Kind;
  SourceLocation StartLoc;
  SourceLocation EndLoc;
};

class ForallStmt : public KitsuneStmt{
public:
  ForallStmt(ForStmt *TheForStmt, CXXForRangeStmt *ForRangeStmt)
  : KitsuneStmt(KitsuneStmt::Forall),
  ForStmt_(TheForStmt),
  ForRangeStmt(ForRangeStmt){}

  ForStmt *getForStmt() { return ForStmt_; }
  CXXForRangeStmt *getForRangeStmt() { return ForRangeStmt; }

  const ForStmt *getForStmt() const { return ForStmt_; }
  const CXXForRangeStmt *getForRangeStmt() const { return ForRangeStmt; }

private:
  ForStmt* ForStmt_;
  CXXForRangeStmt *ForRangeStmt;
};

// +============

}  // end namespace clang

#endif
