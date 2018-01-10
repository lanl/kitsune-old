//===--- Stmt.h - Classes for representing statements -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Stmt interface and subclasses.
//
//===----------------------------------------------------------------------===//

#ifndef KITSUNE_LLVM_CLANG_AST_STMT_H
#define KITSUNE_LLVM_CLANG_AST_STMT_H

#include "clang/AST/Stmt.h"

namespace clang {

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

/*
  static bool classof(const Stmt *T) {
    return T->getStmtClass() == KitsuneStmtClass;
  }
  */

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
  ForallStmt(const ASTContext &C, DeclStmt *IndVar, Expr *Size,
             Stmt *Body, SourceLocation FL, SourceLocation LP,
             SourceLocation RP)
  : KitsuneStmt(KitsuneStmt::Forall){}

  DeclStmt *getIndVar() { return IndVar; }
  Expr *getSize()  { return Size; }
  Stmt *getBody() { return Body; }

private:
  DeclStmt *IndVar;
  Expr *Size;
  Stmt *Body;
};

// +============

}  // end namespace clang

#endif
