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

#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Sema/Kitsune/FleCSIUtility.h"



// -----------------------------------------------------------------------------
// Helper functions: general
// -----------------------------------------------------------------------------

namespace flecsi {

// getMethod
const clang::CXXMethodDecl *getMethod(
   const clang::CallExpr *const ce
) {
   kitsune_debug("getMethod()");
   if (!ce) return nullptr;

   const clang::Decl *const
      cd = ce->getCalleeDecl();
   if (!cd) return nullptr;

   const clang::CXXMethodDecl *const
      md = clang::dyn_cast<clang::CXXMethodDecl>(cd);
   if (!md) return nullptr;

   return md;
}



// getTemplateArgs
const clang::TemplateArgumentList *getTemplateArgs(
   const clang::CallExpr *const ce
) {
   kitsune_debug("getTemplateArgs()");
   if (!ce) return nullptr;

   const clang::CXXMethodDecl *const
      md = getMethod(ce);
   if (!md) return nullptr;

   const clang::TemplateArgumentList *const
      ta = md->getTemplateSpecializationArgs();
   if (!ta) return nullptr;

   return ta;
}



// getTypeArg
clang::QualType getTypeArg(
   const clang::TemplateArgumentList *const ta,
   const std::size_t param
) {
   kitsune_debug("getTypeArg()");
   const clang::TemplateArgument &arg = ta->get(param);
   kitsune_assert(arg.getKind() == clang::TemplateArgument::Type);
   return arg.getAsType();
}

// getIntArg
std::int64_t getIntArg(
   const clang::TemplateArgumentList *const ta,
   const std::size_t param
) {
   kitsune_debug("getIntArg()");
   const clang::TemplateArgument &arg = ta->get(param);
   kitsune_assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getSExtValue();
}

// getUIntArg
std::uint64_t getUIntArg(
   const clang::TemplateArgumentList *const ta,
   const std::size_t param
) {
   kitsune_debug("getUIntArg()");
   const clang::TemplateArgument &arg = ta->get(param);
   kitsune_assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getZExtValue();
}



// getClassDecl
const clang::CXXRecordDecl *getClassDecl(
   const clang::QualType &qt
) {
   kitsune_debug("getClassDecl()");

   const clang::Type *const tp = qt.getTypePtr();
   if (!tp) return nullptr;

   const clang::RecordType *const rt = clang::dyn_cast<clang::RecordType>(tp);
   if (!rt) return nullptr;

   const clang::RecordDecl *rd = rt->getDecl();
   if (!rd) return nullptr;

   const clang::CXXRecordDecl *cd = clang::dyn_cast<clang::CXXRecordDecl>(rd);
   if (!cd) return nullptr;

   return cd;
}



// getName
std::string getName(const clang::NamedDecl *const nd)
{
   kitsune_debug("getName()");
   clang::DeclarationName dn = nd->getDeclName();
   return dn && dn.isIdentifier() ? dn.getAsString() : "";
}

// getQualifiedName
std::string getQualifiedName(const clang::NamedDecl *const nd)
{
   kitsune_debug("getQualifiedName()");
   return nd->getQualifiedNameAsString();
}



// normExpr
const clang::Expr *normExpr(const clang::Expr *const expr)
{
   kitsune_debug("normExpr()");
   const clang::ExprWithCleanups *const
      ec = clang::dyn_cast<clang::ExprWithCleanups>(expr);
   return ec ? ec->getSubExpr() : expr;
}



// isDerivedFrom
bool isDerivedFrom(
   const clang::CXXRecordDecl *const cd,
   const std::string &qualifiedBaseName
) {
   kitsune_debug("isDerivedFrom()");

   if (getQualifiedName(cd) == qualifiedBaseName)
      return true;

   for (auto base : cd->bases()) {
      const clang::CXXRecordDecl *const rd =
         base.getType().getTypePtr()->getAsCXXRecordDecl();
      if (rd && isDerivedFrom(rd, qualifiedBaseName))
         return true;
   }

   return false;
}



// getClassCall
const clang::CallExpr *getClassCall(
   const clang::Expr *const expr,
   const std::string &theclass,
   const std::string &thefunction,
   const int minArgs,
   const int maxArgs_
) {
   kitsune_debug("getClassCall()");

   const int maxArgs = maxArgs_ < 0 ? minArgs : maxArgs_;
   if (!expr) return nullptr;
   const clang::Expr *const e = normExpr(expr);

   const clang::CallExpr *const call = clang::dyn_cast<clang::CallExpr>(e);
   if (!call) return nullptr;

   const clang::CXXMethodDecl *const md = getMethod(call);
   if (!md) return nullptr;

   // class in which the method is defined
   const clang::CXXRecordDecl *const rd = md->getParent();

   const int numArgs = call->getNumArgs();
   if (!(minArgs <= numArgs && numArgs <= maxArgs))
      return nullptr;

   kitsune_debug(std::string("string 1: ") + theclass);
   kitsune_debug(std::string("string 2: ") + getQualifiedName(rd));
   kitsune_debug(std::string("string 3: ") + thefunction);
   kitsune_debug(std::string("string 4: ") + getName(md));

   return theclass == getQualifiedName(rd) && thefunction == getName(md)
      ? call
      : nullptr;
}

}



// -----------------------------------------------------------------------------
// Helper functions: varargs-related
// -----------------------------------------------------------------------------

namespace flecsi {

// getVarArgs
// Here, we receive a CallExpr corresponding to some code like this:
//    namespace::class::foo<bar>(params,__VA_ARGS__);
// and we wish to extract {type,value} from the function's variadic argument
// list. Below, "start" is the number of parameters (called "params," above)
// that precede __VA_ARGS__.

void getVarArgs(
   const clang::CallExpr *const call,
   std::vector<flecsi::FleCSIVarArgTypeValue> &varargs,
   const unsigned start
) {
   kitsune_debug("getVarArgs");

   clang::LangOptions LangOpts;
   LangOpts.CPlusPlus = true;
   const clang::PrintingPolicy Policy(LangOpts);

   const unsigned narg = call->getNumArgs();
   for (unsigned arg = start;  arg < narg;  ++arg) {
      const clang::Expr *const expr = call->getArg(arg);

      if (expr) {
         const std::string type = expr->getType().getAsString();
         std::string s;
         llvm::raw_string_ostream rso(s);
         expr->printPretty(rso, 0, Policy);
         const std::string value = rso.str();
         varargs.push_back(flecsi::FleCSIVarArgTypeValue(type,value));
      } else {
         const std::string type  = "unknown type";
         const std::string value = "unknown value";
         varargs.push_back(flecsi::FleCSIVarArgTypeValue(type,value));
      }
   }
}



// getVarArgsFleCSIFunctionHandle
// This (in contrast to getVarArgs()) is a specialized function for getting
// the __VA_ARGS__ parameters from a FleCSI construct that looks like this:
//    using func = flecsi::execution::function_handle__<
//       return_type,
//       std::tuple<__VA_ARGS__>
//    >
// Here, the varargs are template parameters, not function parameters, and
// therefore just have {type}, not {type,value}.

void getVarArgsFleCSIFunctionHandle(
   const clang::TypeAliasDecl *const alias,
   std::vector<flecsi::FleCSIVarArgType> &varargs
) {
   kitsune_debug("getVarArgsFleCSIFunctionHandle");

   // Consider that our type alias looks like this:
   //    using foo = bar<int,std::tuple<float,double>>

   // Right-hand side:
   //    bar<int,std::tuple<float,double>>
   const clang::ClassTemplateSpecializationDecl *const rhs =
      clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
         alias->getUnderlyingType().getTypePtr()->getAsCXXRecordDecl());

   // Right-hand side's template arguments:
   //    int
   //    std::tuple<float,double>
   const clang::TemplateArgumentList &outer = rhs->getTemplateArgs();

   // The std::tuple, as a QualType
   const clang::QualType tup = outer.get(1).getAsType();

   // The std::tuple, as a ClassTemplateSpecializationDecl
   const clang::ClassTemplateSpecializationDecl *const tuple =
      clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
         tup.getTypePtr()->getAsCXXRecordDecl());

   // The std::tuple's template argument list.
   // Note: tuple is a variadic template class, for which getTemplateArgs()
   // actually gives a "template argument list" with just one argument! The
   // one argument represents the argument pack.
   const clang::TemplateArgumentList &inner = tuple->getTemplateArgs();
   kitsune_assert(inner.size() == 1); // one pack
   kitsune_assert(inner.asArray().size() == 1); // still one pack

   // The "one" argument: the parameter pack
   const clang::TemplateArgument &pack = inner.get(0);
   kitsune_assert(pack.getKind() == clang::TemplateArgument::Pack);

   // Extract the pack's contents. This finally gives us the __VA_ARGS__!
   // Too bad that Clang requires so much digging.
   for (auto iter = pack.pack_begin();  iter != pack.pack_end();  ++iter) {
      const clang::TemplateArgument &arg = *iter;
      // for our example TypeAlias, type == "float" or "double"...
      const std::string type = arg.getAsType().getAsString();
      varargs.push_back(flecsi::FleCSIVarArgType(type));
   }
}

}
