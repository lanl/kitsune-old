
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



// -----------------------------------------------------------------------------
// Helper functions: general
// -----------------------------------------------------------------------------

namespace flecsi {

// getTemplateArgs
const clang::TemplateArgumentList *getTemplateArgs(
   const clang::FunctionDecl *const fd
) {
   kitsune_debug("getTemplateArgs()");

   return fd->getTemplateSpecializationArgs();
}



// getClassDecl
const clang::CXXRecordDecl *getClassDecl(
   clang::QualType qt
) {
   kitsune_debug("getClassDecl()");

   const clang::Type *t = qt.getTypePtr();

   const clang::RecordType *rt = clang::dyn_cast<clang::RecordType>(t);
   if (!rt)
      return nullptr;

   const clang::RecordDecl *rd = rt->getDecl();
   const clang::CXXRecordDecl *cd = clang::dyn_cast<clang::CXXRecordDecl>(rd);
   if (!cd)
      return nullptr;

   return cd;
}



// getMethod
const clang::CXXMethodDecl *
getMethod(const clang::CallExpr *const call)
{
   kitsune_debug("getMethod()");

   const clang::Decl *cd = call->getCalleeDecl();
   if (!cd)
      return nullptr;

   auto md = clang::dyn_cast<clang::CXXMethodDecl>(cd);
   if (!md)
      return nullptr;

   return md;
}



/*
// getIntArg
std::int64_t getIntArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("getIntArg()");
   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getSExtValue();
}
*/



// getUIntArg
std::uint64_t getUIntArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("getUIntArg()");
   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Integral);
   return arg.getAsIntegral().getZExtValue();
}



// getTypeArg
clang::QualType getTypeArg(
   const clang::TemplateArgumentList *const args,
   const std::size_t param
) {
   kitsune_debug("getTypeArg()");
   const clang::TemplateArgument &arg = args->get(param);
   assert(arg.getKind() == clang::TemplateArgument::Type);
   return arg.getAsType();
}

}



// -----------------------------------------------------------------------------
// Helper functions: varargs-related
// -----------------------------------------------------------------------------

namespace flecsi {

// getVarArgsFromFunctionParameters
// Function parameters: have type and value, so use FlecsiVarArgTypeValue.
void getVarArgsFromFunctionParameters(
   const clang::CallExpr *const call,
   std::vector<flecsi::FlecsiVarArgTypeValue> &varargs,
   const unsigned start = 0
) {
   clang::LangOptions LangOpts;
   LangOpts.CPlusPlus = true;
   clang::PrintingPolicy Policy(LangOpts);

   const unsigned narg = call->getNumArgs();
   for (unsigned arg = start;  arg < narg;  ++arg) {
      const clang::Expr *const expr = call->getArg(arg);

      if (expr == nullptr) {
         const std::string type  = "unknown type";
         const std::string value = "unknown value";
         varargs.push_back(flecsi::FlecsiVarArgTypeValue(type,value));
      } else {
         // type
         const std::string type = expr->getType().getAsString();

         // value
         std::string s;
         llvm::raw_string_ostream rso(s);
         expr->printPretty(rso, 0, Policy);
         const std::string value = rso.str();

         varargs.push_back(flecsi::FlecsiVarArgTypeValue(type,value));
      }
   }
}



// getVarArgsFromFunctionHandle
// Template parameters: have type only, so use FlecsiVarArgType.
void getVarArgsFromFunctionHandle(
   const clang::TypeAliasDecl *const alias,
   std::vector<flecsi::FlecsiVarArgType> &varargs
) {
   // qqq clean up this function

   // Consider a type alias like this:
   //    using foo = bar<int,tuple<float,double>>
   // where bar is a regular class template with two template arguments,
   // and tuple is std::tuple - with a variadic template argument pack.

   // Right-hand side of the type alias:
   //    bar<int,tuple<float,double>>
   // in our hypothetic example.
   const clang::ClassTemplateSpecializationDecl *const rhs =
      clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
         alias->getUnderlyingType().getTypePtr()->getAsCXXRecordDecl()
      );

   // Template arguments of the right-hand side:
   //    int
   //    tuple<float,double>
   // in our hypothetic example.
   const clang::TemplateArgumentList &outer = rhs->getTemplateArgs();

   // FYI
   kitsune_print(outer.size());
   kitsune_print(outer.get(0).getAsType().getAsString());
   kitsune_print(outer.get(1).getAsType().getAsString());

   // Now, let's dig into the tuple. It turns out that getting its arguments
   // involves a different process than what we used above, due to its being
   // a variadic template class.

   // First, get the tuple itself, as a QualType
   clang::QualType qttup = outer.get(1).getAsType();

   // Next, get the tuple as a ClassTemplateSpecializationDecl
   const clang::ClassTemplateSpecializationDecl *const tuple =
      clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(
         qttup.getTypePtr()->getAsCXXRecordDecl()
      );

   // FYI
   kitsune_print(tuple);

   // Template argument of the tuple. Because tuple is a variadic template
   // class, getTemplateArgs() in fact gives a "TemplateArgumentList" with
   // just one argument, which itself represents the argument pack.
   const clang::TemplateArgumentList &inner = tuple->getTemplateArgs();

   // FYI
   kitsune_print(inner.size()); // 1 (the pack)
   kitsune_print(inner.asArray().size()); // 1
   ///inner.get(0).dump();

   // The following should be a pointer to a const TemplateArgument
   (void)&inner.get(0);

   const clang::TemplateArgument &pack = inner.get(0);
   kitsune_print(pack.getKind());  // tells me this IS A PACK

   // Confusingly, even though it's a pack (according to the above diagnotic),
   // both of these return 0 (false).
   kitsune_print(pack.containsUnexpandedParameterPack());
   kitsune_print(pack.isPackExpansion());
   kitsune_print(pack.pack_size());

   // Let's see if we can successfully iterate over a pack
   for (auto iter = pack.pack_begin();  iter != pack.pack_end();  ++iter) {
      const clang::TemplateArgument &arg = *iter;
      const std::string type = arg.getAsType().getAsString();
      kitsune_print(type);
      varargs.push_back(flecsi::FlecsiVarArgType(type));
   }
}

}



// -----------------------------------------------------------------------------
// Helper functions: general
// qqq Maybe combine with earlier ones
// -----------------------------------------------------------------------------

namespace flecsi {

// getName
std::string getName(const clang::NamedDecl *const nd)
{
   kitsune_debug("getName()");
   clang::DeclarationName dn = nd->getDeclName();
   if (!dn || !dn.isIdentifier())
      return "";
   return dn.getAsString();
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
   if (auto ec = clang::dyn_cast<clang::ExprWithCleanups>(expr))
      return ec->getSubExpr();
   return expr;
}



// isDerivedFrom
bool isDerivedFrom(
   const clang::CXXRecordDecl *const cd,
   const std::string &qualifiedBaseName
) {
   kitsune_debug("isDerivedFrom()");

   if (getQualifiedName(cd) == qualifiedBaseName)
      return true;

   for (auto bi : cd->bases()) {
      const clang::CXXRecordDecl *bcd =
         bi.getType().getTypePtr()->getAsCXXRecordDecl();
      if (bcd && isDerivedFrom(bcd, qualifiedBaseName))
         return true;
   }

   return false;
}



// getClassCall, 5/6-argument
const clang::CallExpr *getClassCall(
   const clang::Expr *expr,
   const std::string &className,
   const std::string &call,
   const clang::CXXRecordDecl * &rd,
   const int minArgs,
   const int maxArgs_ = -1
) {
   kitsune_debug("getClassCall(), 5/6-argument");

   const int maxArgs = maxArgs_ < 0 ? minArgs : maxArgs_;
   if (!expr)
      return nullptr;
   const clang::Expr *const e = normExpr(expr);

   auto callexpr = clang::dyn_cast<clang::CallExpr>(e);
   if (!callexpr)
      return nullptr;

   const clang::CXXMethodDecl *md = getMethod(callexpr);
   if (!md)
      return nullptr;

   rd = md->getParent();

   int numArgs = callexpr->getNumArgs();
   kitsune_print(numArgs);
   if (numArgs < minArgs || numArgs > maxArgs)
      return nullptr;

   kitsune_debug(std::string("string 1: ") + className);
   kitsune_debug(std::string("string 2: ") + getQualifiedName(rd));
   kitsune_debug(std::string("string 3: ") + call);
   kitsune_debug(std::string("string 4: ") + getName(md));

   return className == getQualifiedName(rd) && call == getName(md)
      ? callexpr
      : nullptr;
}



// getClassCall, 4-argument
const clang::CallExpr *getClassCall(
   const clang::Expr *const expr,
   const std::string &className,
   const std::string &call,
   const clang::CXXRecordDecl * &rd
) {
   kitsune_debug("getClassCall(), 3-argument");

   return getClassCall(
      expr, className, call, rd,
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::max()
   );
}

}



// -----------------------------------------------------------------------------
// VisitVarDecl
// -----------------------------------------------------------------------------

namespace flecsi {

bool PreprocessorASTVisitor::
VisitVarDecl(const clang::VarDecl *const var)
{
   kitsune_debug("PreprocessorASTVisitor::VisitVarDecl()");
   kitsune_print(var->getNameAsString());

   // The following was here when I (Martin) took over the code, but it appears
   // to have little effect on code timing - for my test codes, at least, which
   // may not be representative of more-general FleCSI codes. Given that this
   // check necessarily creates a maintenance issue (if the names of variables
   // created by certain FleCSI macros are changed), I'll remove it for now.
   /*
   if (!var->getName().endswith("_registered"))
      return true;
   */

   // Associated MacroInvocation
   const MacroInvocation *const iptr = prep.getInvocation(var->getLocStart());
   if (!iptr)
      return true;
   const MacroInvocation &invoke = *iptr;
   const clang::Token token = invoke.token;

   std::string name = tostr(sema,token);
   kitsune_print(name);
   const clang::CXXRecordDecl *rd;

   const clang::Expr *const expr = var->getInit();



   // flecsi_register_task_simple(task, processor, launch)
   if (name == "flecsi_register_task_simple") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::execution::task_interface__", "register_task",
         rd, 3
      );

      if (call != nullptr) {
         flecsi::FlecsiRegisterTaskSimple c(sema,token);
         std::size_t pos = 0;
         c.task      = invoke.str(sema,pos++);
         c.processor = invoke.str(sema,pos++);
         c.launch    = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterTaskSimple.push_back(c);
      }
   }



   // flecsi_register_task(task, nspace, processor, launch)
   if (name == "flecsi_register_task") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::execution::task_interface__", "register_task",
         rd, 3
      );

      if (call != nullptr) {
         flecsi::FlecsiRegisterTask c(sema,token);
         std::size_t pos = 0;
         c.task      = invoke.str(sema,pos++);
         c.nspace    = invoke.str(sema,pos++);
         c.processor = invoke.str(sema,pos++);
         c.launch    = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterTask.push_back(c);
      }
   }



   // flecsi_register_mpi_task_simple(task)
   if (name == "flecsi_register_mpi_task_simple") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::execution::task_interface__", "register_task",
         rd, 3
      );

      if (call != nullptr) {
         flecsi::FlecsiRegisterMPITaskSimple c(sema,token);
         std::size_t pos = 0;
         c.task = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterMPITaskSimple.push_back(c);
      }
   }



   // flecsi_register_mpi_task(task, nspace)
   if (name == "flecsi_register_mpi_task") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::execution::task_interface__", "register_task",
         rd, 3
      );

      if (call != nullptr) {
         flecsi::FlecsiRegisterMPITask c(sema,token);
         std::size_t pos = 0;
         c.task   = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterMPITask.push_back(c);
      }
   }



   // flecsi_register_data_client(client_type, nspace, name)
   if (name == "flecsi_register_data_client") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::data::data_client_interface__", "register_data_client",
         rd, 1
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         const clang::QualType qt = getTypeArg(ta,0);
         const clang::CXXRecordDecl *const cd = getClassDecl(qt);
         if (!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")) {
            sema.Diag(
               invoke.loc(0,0),
               clang::diag::err_flecsi_not_a_data_client);
            return true;
         }

         flecsi::FlecsiRegisterDataClient c(sema,token);
         std::size_t pos = 0;
         c.client_type = invoke.str(sema,pos++);
         c.nspace      = invoke.str(sema,pos++);
         c.name        = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterDataClient.push_back(c);
      }
   }



   // flecsi_register_field(client_type, nspace, name, data_type,
   //                       storage_class, versions, ...)
   if (name == "flecsi_register_field") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::data::field_interface__", "register_field",
         rd, 1
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiRegisterField c(sema,token);
         std::size_t pos = 0;
         c.client_type   = invoke.str(sema,pos++);
         c.nspace        = invoke.str(sema,pos++);
         c.name          = invoke.str(sema,pos++);
         c.data_type     = invoke.str(sema,pos++);
         c.storage_class = invoke.str(sema,pos++);
         c.versions    = getUIntArg(ta,5); // 5 = template argument position
         c.index_space = getUIntArg(ta,6);
         prep.yaml().FlecsiRegisterField.push_back(c);
      }
   }



   // flecsi_register_global(nspace, name, data_type, versions, ...)
   if (name == "flecsi_register_global") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::data::field_interface__", "register_field",
         rd, 1
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiRegisterGlobal c(sema,token);
         std::size_t pos = 0;
         c.nspace    = invoke.str(sema,pos++);
         c.name      = invoke.str(sema,pos++);
         c.data_type = invoke.str(sema,pos++);
         c.versions  = getUIntArg(ta,5);
         prep.yaml().FlecsiRegisterGlobal.push_back(c);
      }
   }



   // flecsi_register_color(nspace, name, data_type, versions, ...)
   if (name == "flecsi_register_color") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::data::field_interface__", "register_field",
         rd, 1
      );

      if (call != nullptr) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiRegisterColor c(sema,token);
         std::size_t pos = 0;
         c.nspace    = invoke.str(sema,pos++);
         c.name      = invoke.str(sema,pos++);
         c.data_type = invoke.str(sema,pos++);
         c.versions  = getUIntArg(ta,5);
         prep.yaml().FlecsiRegisterColor.push_back(c);
      }
   }



   // flecsi_register_function(func, nspace)
   if (name == "flecsi_register_function") {
      const clang::CallExpr *const call = getClassCall(
         expr, "flecsi::execution::function_interface__", "register_function",
         rd, 0
      );

      if (call != nullptr) {
         flecsi::FlecsiRegisterFunction c(sema,token);
         std::size_t pos = 0;
         c.func   = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterFunction.push_back(c);
      }
   }



   return true;
}

}



// -----------------------------------------------------------------------------
// VisitCallExpr
// -----------------------------------------------------------------------------

namespace flecsi {

bool PreprocessorASTVisitor::
VisitCallExpr(const clang::CallExpr *const call)
{
   kitsune_debug("PreprocessorASTVisitor::VisitCallExpr()");

   const clang::Decl *cd = call->getCalleeDecl();
   if (!cd)
      return true;

   auto fd = clang::dyn_cast<clang::FunctionDecl>(cd);
   if (!fd)
      return true;

   clang::DeclarationName dn = fd->getDeclName();
   if (!dn || !dn.isIdentifier())
      return true;

   const MacroInvocation *const iptr = prep.getInvocation(call->getLocStart());
   if (!iptr)
      return true;
   const MacroInvocation &invoke = *iptr;
   const clang::Token token = invoke.token;

   std::string name = tostr(sema,token);
   kitsune_print(name);
   const clang::CXXRecordDecl *rd;



   // flecsi_execute_task_simple(task, launch, ...)
   if (name == "flecsi_execute_task_simple") {
      if (getClassCall(
         call, "flecsi::execution::task_interface__", "execute_task",
         rd
      )) {
         flecsi::FlecsiExecuteTaskSimple c(sema,token);
         std::size_t pos = 0;
         c.task   = invoke.str(sema,pos++);
         c.launch = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs);
         prep.yaml().FlecsiExecuteTaskSimple.push_back(c);
      }
   }

   // flecsi_execute_task(task, nspace, launch, ...)
   if (name == "flecsi_execute_task") {
      if (getClassCall(
         call, "flecsi::execution::task_interface__", "execute_task",
         rd
      )) {
         flecsi::FlecsiExecuteTask c(sema,token);
         std::size_t pos = 0;
         c.task   = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         c.launch = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs);
         prep.yaml().FlecsiExecuteTask.push_back(c);
      }
   }

   // flecsi_execute_mpi_task_simple(task, ...)
   if (name == "flecsi_execute_mpi_task_simple") {
      if (getClassCall(
         call, "flecsi::execution::task_interface__", "execute_task",
         rd
      )) {
         flecsi::FlecsiExecuteMPITaskSimple c(sema,token);
         std::size_t pos = 0;
         c.task = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs);
         prep.yaml().FlecsiExecuteMPITaskSimple.push_back(c);
      }
   }

   // flecsi_execute_mpi_task(task, nspace, ...)
   if (name == "flecsi_execute_mpi_task") {
      if (getClassCall(
         call, "flecsi::execution::task_interface__", "execute_task",
         rd
      )) {
         flecsi::FlecsiExecuteMPITask c(sema,token);
         std::size_t pos = 0;
         c.task   = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs);
         prep.yaml().FlecsiExecuteMPITask.push_back(c);
      }
   }



   // flecsi_register_global_object(index, nspace, type)
   if (name == "flecsi_register_global_object") {
      if (getClassCall(
         call, "flecsi::execution::context__", "register_global_object",
         rd
      )) {
         flecsi::FlecsiRegisterGlobalObject c(sema,token);
         std::size_t pos = 0;
         c.index  = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         c.type   = invoke.str(sema,pos++);
         prep.yaml().FlecsiRegisterGlobalObject.push_back(c);
      }
   }

   // flecsi_set_global_object(index, nspace, type, obj)
   if (name == "flecsi_set_global_object") {
      if (getClassCall(
         call, "flecsi::execution::context__", "set_global_object",
         rd
      )) {
         flecsi::FlecsiSetGlobalObject c(sema,token);
         std::size_t pos = 0;
         c.index  = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         c.type   = invoke.str(sema,pos++);
         c.obj    = invoke.str(sema,pos++);
         prep.yaml().FlecsiSetGlobalObject.push_back(c);
      }
   }

   // flecsi_initialize_global_object(index, nspace, type, ...)
   if (name == "flecsi_initialize_global_object") {
      if (getClassCall(
         call, "flecsi::execution::context__", "initialize_global_object",
         rd
      )) {
         flecsi::FlecsiInitializeGlobalObject c(sema,token);
         std::size_t pos = 0;
         c.index  = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         c.type   = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs, 1);
         prep.yaml().FlecsiInitializeGlobalObject.push_back(c);
      }
   }

   // flecsi_get_global_object(index, nspace, type)
   if (name == "flecsi_get_global_object") {
      if (getClassCall(
         call, "flecsi::execution::context__", "get_global_object",
         rd
      )) {
         flecsi::FlecsiGetGlobalObject c(sema,token);
         std::size_t pos = 0;
         c.index  = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         c.type   = invoke.str(sema,pos++);
         prep.yaml().FlecsiGetGlobalObject.push_back(c);
      }
   }

   // flecsi_execute_function(handle, ...)
   if (name == "flecsi_execute_function") {
      if (getClassCall(
         call, "flecsi::execution::function_interface__", "execute_function",
         rd
      )) {
         flecsi::FlecsiExecuteFunction c(sema,token);
         std::size_t pos = 0;
         c.handle = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs, 1);
         prep.yaml().FlecsiExecuteFunction.push_back(c);
      }
   }

   // flecsi_function_handle(func, nspace)
   if (name == "flecsi_function_handle") {
      kitsune_debug("almost found 2: function handle");

      if (getClassCall(
         call, "flecsi::utils::const_string_t", "hash",
         rd
      )) {
         flecsi::FlecsiFunctionHandle c(sema,token);
         std::size_t pos = 0;
         c.func   = invoke.str(sema,pos++);
         c.nspace = invoke.str(sema,pos++);
         prep.yaml().FlecsiFunctionHandle.push_back(c);
      }
   }



   // flecsi_get_handle
   if (name == "flecsi_get_handle") {
      if (getClassCall(
         call, "flecsi::data::field_interface__", "get_handle",
         rd, 1
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiGetHandle c(sema,token);
         std::size_t pos = 0;
         c.client_handle = invoke.str(sema,pos++);
         c.nspace        = invoke.str(sema,pos++);
         c.name          = invoke.str(sema,pos++);
         c.data_type     = invoke.str(sema,pos++);
         c.storage_class = invoke.str(sema,pos++);
         c.version       = getUIntArg(ta,5); // 5 = template argument position
         prep.yaml().FlecsiGetHandle.push_back(c);
      }
   }

   // flecsi_get_client_handle
   if (name == "flecsi_get_client_handle") {
      if (getClassCall(
         call, "flecsi::data::data_client_interface__", "get_client_handle",
         rd
      )) {
         flecsi::FlecsiGetClientHandle c(sema,token);
         std::size_t pos = 0;
         c.client_type = invoke.str(sema,pos++);
         c.nspace      = invoke.str(sema,pos++);
         c.name        = invoke.str(sema,pos++);
         prep.yaml().FlecsiGetClientHandle.push_back(c);
      }
   }



   // flecsi_get_mutator
   if (name == "flecsi_get_mutator") {
      if (getClassCall(
         call, "flecsi::data::field_interface__", "get_mutator",
         rd, 2
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiGetMutator c(sema,token);
         std::size_t pos = 0;
         c.client_handle = invoke.str(sema,pos++);
         c.nspace        = invoke.str(sema,pos++);
         c.name          = invoke.str(sema,pos++);
         c.data_type     = invoke.str(sema,pos++);
         c.storage_class = invoke.str(sema,pos++);
         c.version       = getUIntArg(ta,5); // 5 = template argument position
         pos++;  // version pulled via other means; skip to slots
         c.slots         = invoke.str(sema,pos++);
         prep.yaml().FlecsiGetMutator.push_back(c);
      }
   }

   // flecsi_get_global
   if (name == "flecsi_get_global") {
      if (getClassCall( // this macro invokes the flecsi_get_handle macro...
         call, "flecsi::data::field_interface__", "get_handle",
         rd, 1
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiGetGlobal c(sema,token);
         std::size_t pos = 0;
         c.nspace    = invoke.str(sema,pos++);
         c.name      = invoke.str(sema,pos++);
         c.data_type = invoke.str(sema,pos++);
         c.version   = getUIntArg(ta,5); // 5 = template argument position
         prep.yaml().FlecsiGetGlobal.push_back(c);
      }
   }

   // flecsi_get_color
   if (name == "flecsi_get_color") {
      if (getClassCall( // this macro invokes the flecsi_get_handle macro...
         call, "flecsi::data::field_interface__", "get_handle",
         rd, 1
      )) {
         const clang::CXXMethodDecl        *const md = getMethod(call);
         const clang::TemplateArgumentList *const ta = getTemplateArgs(md);
         flecsi::FlecsiGetColor c(sema,token);
         std::size_t pos = 0;
         c.nspace    = invoke.str(sema,pos++);
         c.name      = invoke.str(sema,pos++);
         c.data_type = invoke.str(sema,pos++);
         c.version   = getUIntArg(ta,5); // 5 = template argument position
         prep.yaml().FlecsiGetColor.push_back(c);
      }
   }



   /*
   // flecsi_get_handles
   if (name == "flecsi_get_handles") {
      if (getClassCall(
         call, "flecsi::data::field_interface__", "get_handles",
         rd, 2, std::numeric_limits<int>::max()
      )) {
         flecsi::FlecsiGetHandles c(sema,token);
         std::size_t pos = 0;
         c.client        = invoke.str(sema,pos++);
         c.nspace        = invoke.str(sema,pos++);
         c.data_type     = invoke.str(sema,pos++);
         c.storage_class = invoke.str(sema,pos++);
         c.version       = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs, 2);
         prep.yaml().FlecsiGetHandles.push_back(c);
      }
   }

   // flecsi_get_handles_all
   if (name == "flecsi_get_handles_all") {
      if (getClassCall(
         call, "flecsi::data::field_interface__", "get_handles",
         rd, 2, std::numeric_limits<int>::max()
      )) {
         flecsi::FlecsiGetHandlesAll c(sema,token);
         std::size_t pos = 0;
         c.client        = invoke.str(sema,pos++);
         c.data_type     = invoke.str(sema,pos++);
         c.storage_class = invoke.str(sema,pos++);
         c.version       = invoke.str(sema,pos++);
         getVarArgsFromFunctionParameters(call, c.varargs, 2);
         prep.yaml().FlecsiGetHandlesAll.push_back(c);
      }
   }
   */



   return true;
}

}



// -----------------------------------------------------------------------------
// VisitTypeAliasDecl
// -----------------------------------------------------------------------------

namespace flecsi {

bool PreprocessorASTVisitor::
VisitTypeAliasDecl(const clang::TypeAliasDecl *const ta)
{
   kitsune_debug("PreprocessorASTVisitor::VisitTypeAliasDecl()");
   kitsune_print(ta->getNameAsString());

   // Associated MacroInvocation
   const MacroInvocation *const iptr = prep.getInvocation(ta->getLocStart());
   if (!iptr)
      return true;
   const MacroInvocation &invoke = *iptr;
   const clang::Token token = invoke.token;

   std::string name = tostr(sema,token);
   kitsune_print(name);

   // flecsi_define_function_type(func, return_type, ...)
   if (name == "flecsi_define_function_type") {
      flecsi::FlecsiDefineFunctionType c(sema,token);
      std::size_t pos = 0;
      c.func        = invoke.str(sema,pos++);
      c.return_type = invoke.str(sema,pos++);
      getVarArgsFromFunctionHandle(ta, c.varargs);
      prep.yaml().FlecsiDefineFunctionType.push_back(c);
   }

   return true;
}

}
