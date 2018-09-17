/**
  ***************************************************************************
  * Copyright (c) 2018, Los Alamos National Security, LLC.
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

#include "clang/Sema/SemaDiagnostic.h"
#include "clang/Sema/Kitsune/FleCSIUtility.h"
#include "clang/Sema/Kitsune/FleCSIPreprocessor.h"
#include "clang/Sema/Kitsune/FleCSIPreprocessorASTVisitor.h"



// -----------------------------------------------------------------------------
// Helper function: match()
// -----------------------------------------------------------------------------

namespace {

bool match(
   const clang::CallExpr *&call,
   const clang::Expr *const expr,
   const std::string &name,
   const std::string &macroname,
   const std::string &theclass = "",
   const std::string &thefunction = "",
   int min = -1,
   int max = -1
) {
   call = nullptr;

   if (name != macroname)
      return false;

   if (theclass == "" && thefunction == "")
      return true;

   if (min == -1 && max == -1) {
      min = std::numeric_limits<int>::min();
      max = std::numeric_limits<int>::max();
   } else if (max == -1)
      max = min;

   return call = flecsi::getClassCall(expr, theclass, thefunction, min, max);
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

   // associated macro invocation (if any); token name
   const MacroInvocation *const iptr = prep.get_invocation(var->getLocStart());
   if (!iptr) return true;
   const MacroInvocation &macro = *iptr;
   const std::string name = tostr(sema,macro.token);

   // for use below
   const clang::Expr *const expr = var->getInit();
   std::size_t pos = 0;
   const clang::CallExpr *call; // tbd


   // ------------------------
   // Top-level driver
   // interface
   // ------------------------

   // flecsi_register_program(program)
   if (match(call, expr, name, "flecsi_register_program")) {
      flecsi::FleCSIRegisterProgram c(sema,macro.token);
      c.program = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterProgram.push_back(c);
   }

   // flecsi_register_top_level_driver(driver)
   if (match(call, expr, name, "flecsi_register_top_level_driver")) {
      flecsi::FleCSIRegisterTopLevelDriver c(sema,macro.token);
      c.driver = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterTopLevelDriver.push_back(c);
   }


   /*
   // ------------------------
   // Reduction
   // interface
   // ------------------------

   // flecsi_register_reduction_operation(name, operation_type)
   if (match(call, expr, name, "flecsi_register_reduction_operation",
      "flecsi::execution::task_interface__",
      "register_reduction_operation", 0)) {
      flecsi::FleCSIRegisterReductionOperation c(sema,macro.token);
      c.name           = macro.str(sema,pos++);
      c.operation_type = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterReductionOperation.push_back(c);
   }
   */


   // ------------------------
   // Task registration
   // interface
   // ------------------------

   // flecsi_register_task(task, nspace, processor, launch)
   if (match(call, expr, name, "flecsi_register_task",
      "flecsi::execution::task_interface__", "register_task", 3)) {
      flecsi::FleCSIRegisterTask c(sema,macro.token);
      c.task      = macro.str(sema,pos++);
      c.nspace    = macro.str(sema,pos++);
      c.processor = macro.str(sema,pos++);
      c.launch    = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterTask.push_back(c);
   }

   // flecsi_register_mpi_task(task, nspace)
   if (match(call, expr, name, "flecsi_register_mpi_task",
      "flecsi::execution::task_interface__", "register_task", 3)) {
      flecsi::FleCSIRegisterMPITask c(sema,macro.token);
      c.task   = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterMPITask.push_back(c);
   }

   // flecsi_register_task_simple(task, processor, launch)
   if (match(call, expr, name, "flecsi_register_task_simple",
      "flecsi::execution::task_interface__", "register_task", 3)) {
      flecsi::FleCSIRegisterTaskSimple c(sema,macro.token);
      c.task      = macro.str(sema,pos++);
      c.processor = macro.str(sema,pos++);
      c.launch    = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterTaskSimple.push_back(c);
   }

   // flecsi_register_mpi_task_simple(task)
   if (match(call, expr, name, "flecsi_register_mpi_task_simple",
      "flecsi::execution::task_interface__", "register_task", 3)) {
      flecsi::FleCSIRegisterMPITaskSimple c(sema,macro.token);
      c.task = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterMPITaskSimple.push_back(c);
   }


   // ------------------------
   // Function
   // interface
   // ------------------------

   // flecsi_define_function_type(func, return_type, ...)
   // See VisitTypeAliasDecl()

   // flecsi_register_function(func, nspace)
   if (match(call, expr, name, "flecsi_register_function",
      "flecsi::execution::function_interface__", "register_function", 0)) {
      flecsi::FleCSIRegisterFunction c(sema,macro.token);
      c.func   = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterFunction.push_back(c);
   }

   // flecsi_function_handle(func, nspace)
   // See VisitCallExpr()

   // flecsi_execute_function(handle, ...)
   // See VisitCallExpr()


   // ------------------------
   // Other
   // ------------------------

   // flecsi_register_data_client(client_type, nspace, name)
   if (match(call, expr, name, "flecsi_register_data_client",
      "flecsi::data::data_client_interface__", "register_data_client", 1)) {

      const clang::TemplateArgumentList *const ta = getTemplateArgs(call);
      const clang::QualType qt = getTypeArg(ta,0);
      const clang::CXXRecordDecl *const cd = getClassDecl(qt);

      if (!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")) {
         sema.Diag(
            macro.loc(0,0),
            clang::diag::err_flecsi_not_a_data_client);
         return true;
      }

      flecsi::FleCSIRegisterDataClient c(sema,macro.token);
      c.client_type = macro.str(sema,pos++);
      c.nspace      = macro.str(sema,pos++);
      c.name        = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterDataClient.push_back(c);
   }

   // flecsi_register_field(client_type, nspace, name, data_type,
   //                       storage_class, versions, ...)
   if (match(call, expr, name, "flecsi_register_field",
      "flecsi::data::field_interface__", "register_field", 1)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(call);
      flecsi::FleCSIRegisterField c(sema,macro.token);
      c.client_type   = macro.str(sema,pos++);
      c.nspace        = macro.str(sema,pos++);
      c.name          = macro.str(sema,pos++);
      c.data_type     = macro.str(sema,pos++);
      c.storage_class = macro.str(sema,pos++);
      c.versions    = getUIntArg(ta,5); // 5 = template argument position
      c.index_space = getUIntArg(ta,6);
      prep.yaml().FleCSIRegisterField.push_back(c);
   }

   // flecsi_register_global(nspace, name, data_type, versions, ...)
   if (match(call, expr, name, "flecsi_register_global",
      "flecsi::data::field_interface__", "register_field", 1)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(call);
      flecsi::FleCSIRegisterGlobal c(sema,macro.token);
      c.nspace    = macro.str(sema,pos++);
      c.name      = macro.str(sema,pos++);
      c.data_type = macro.str(sema,pos++);
      c.versions  = getUIntArg(ta,5);
      prep.yaml().FleCSIRegisterGlobal.push_back(c);
   }

   // flecsi_register_color(nspace, name, data_type, versions, ...)
   if (match(call, expr, name, "flecsi_register_color",
      "flecsi::data::field_interface__", "register_field", 1)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(call);
      flecsi::FleCSIRegisterColor c(sema,macro.token);
      c.nspace    = macro.str(sema,pos++);
      c.name      = macro.str(sema,pos++);
      c.data_type = macro.str(sema,pos++);
      c.versions  = getUIntArg(ta,5);
      prep.yaml().FleCSIRegisterColor.push_back(c);
   }

   return true;
}

}



// -----------------------------------------------------------------------------
// VisitCallExpr
// -----------------------------------------------------------------------------

namespace flecsi {

bool PreprocessorASTVisitor::
VisitCallExpr(const clang::CallExpr *const expr)
{
   kitsune_debug("PreprocessorASTVisitor::VisitCallExpr()");

   // associated macro invocation (if any); token name
   const MacroInvocation *const iptr = prep.get_invocation(expr->getLocStart());
   if (!iptr) return true;
   const MacroInvocation &macro = *iptr;
   const std::string name = tostr(sema,macro.token);

   // for use below
   std::size_t pos = 0;
   const clang::CallExpr *call; // tbd


   // ------------------------
   // Object
   // interface
   // ------------------------

   // flecsi_register_global_object(index, nspace, type)
   if (match(call, expr, name, "flecsi_register_global_object",
      "flecsi::execution::context__", "register_global_object")) {
      flecsi::FleCSIRegisterGlobalObject c(sema,macro.token);
      c.index  = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      c.type   = macro.str(sema,pos++);
      prep.yaml().FleCSIRegisterGlobalObject.push_back(c);
   }

   // flecsi_initialize_global_object(index, nspace, type, ...)
   if (match(call, expr, name, "flecsi_initialize_global_object",
      "flecsi::execution::context__", "initialize_global_object")) {
      flecsi::FleCSIInitializeGlobalObject c(sema,macro.token);
      c.index  = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      c.type   = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs, 1);
      prep.yaml().FleCSIInitializeGlobalObject.push_back(c);
   }

   // flecsi_get_global_object(index, nspace, type)
   if (match(call, expr, name, "flecsi_get_global_object",
      "flecsi::execution::context__", "get_global_object")) {
      flecsi::FleCSIGetGlobalObject c(sema,macro.token);
      c.index  = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      c.type   = macro.str(sema,pos++);
      prep.yaml().FleCSIGetGlobalObject.push_back(c);
   }

   // flecsi_set_global_object(index, nspace, type, obj)
   if (match(call, expr, name, "flecsi_set_global_object",
      "flecsi::execution::context__", "set_global_object")) {
      flecsi::FleCSISetGlobalObject c(sema,macro.token);
      c.index  = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      c.type   = macro.str(sema,pos++);
      c.obj    = macro.str(sema,pos++);
      prep.yaml().FleCSISetGlobalObject.push_back(c);
   }


   // ------------------------
   // Task execution
   // interface
   // ------------------------

   // Note: for flecsi_color[s], I specifically needed the last two match()
   // arguments, as opposed to having calls like this:
   //    match(call, expr, name, "flecsi_color")
   //    match(call, expr, name, "flecsi_colors")
   // That's because both of those macros, while simple, actually generate
   // two CallExprs in the AST: one to instance(), and the other to color()
   // or colors(). Without the additional check for the exact name of the
   // function being called in the "call expression" (CallExpr), each macro
   // invocation inadvertently generated two identical entries in the YAML.

   // flecsi_color
   if (match(call, expr, name, "flecsi_color",
      "flecsi::execution::context__", "color")) {
      flecsi::FleCSIColor c(sema,macro.token);
      prep.yaml().FleCSIColor.push_back(c);
   }

   // flecsi_colors
   if (match(call, expr, name, "flecsi_colors",
      "flecsi::execution::context__", "colors")) {
      flecsi::FleCSIColors c(sema,macro.token);
      prep.yaml().FleCSIColors.push_back(c);
   }

   // flecsi_execute_task(task, nspace, launch, ...)
   if (match(call, expr, name, "flecsi_execute_task",
      "flecsi::execution::task_interface__", "execute_task")) {
      flecsi::FleCSIExecuteTask c(sema,macro.token);
      c.task   = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      c.launch = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs);
      prep.yaml().FleCSIExecuteTask.push_back(c);
   }

   // flecsi_execute_mpi_task(task, nspace, ...)
   if (match(call, expr, name, "flecsi_execute_mpi_task",
      "flecsi::execution::task_interface__", "execute_task")) {
      flecsi::FleCSIExecuteMPITask c(sema,macro.token);
      c.task   = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs);
      prep.yaml().FleCSIExecuteMPITask.push_back(c);
   }

   // flecsi_execute_task_simple(task, launch, ...)
   if (match(call, expr, name, "flecsi_execute_task_simple",
      "flecsi::execution::task_interface__", "execute_task")) {
      flecsi::FleCSIExecuteTaskSimple c(sema,macro.token);
      c.task   = macro.str(sema,pos++);
      c.launch = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs);
      prep.yaml().FleCSIExecuteTaskSimple.push_back(c);
   }

   // flecsi_execute_mpi_task_simple(task, ...)
   if (match(call, expr, name, "flecsi_execute_mpi_task_simple",
      "flecsi::execution::task_interface__", "execute_task")) {
      flecsi::FleCSIExecuteMPITaskSimple c(sema,macro.token);
      c.task = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs);
      prep.yaml().FleCSIExecuteMPITaskSimple.push_back(c);
   }


   // ------------------------
   // Function
   // interface
   // ------------------------

   // flecsi_define_function_type(func, return_type, ...)
   // See VisitTypeAliasDecl()

   // flecsi_register_function(func, nspace)
   // See VisitVarDecl()

   // flecsi_function_handle(func, nspace)
   if (match(call, expr, name, "flecsi_function_handle",
      "flecsi::utils::const_string_t", "hash")) {
      flecsi::FleCSIFunctionHandle c(sema,macro.token);
      c.func   = macro.str(sema,pos++);
      c.nspace = macro.str(sema,pos++);
      prep.yaml().FleCSIFunctionHandle.push_back(c);
   }

   // flecsi_execute_function(handle, ...)
   if (match(call, expr, name, "flecsi_execute_function",
      "flecsi::execution::function_interface__", "execute_function")) {
      flecsi::FleCSIExecuteFunction c(sema,macro.token);
      c.handle = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs, 1);
      prep.yaml().FleCSIExecuteFunction.push_back(c);
   }


   // ------------------------
   // Other
   // ------------------------

   // flecsi_get_handle
   if (match(call, expr, name, "flecsi_get_handle",
      "flecsi::data::field_interface__", "get_handle", 1)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(expr);
      flecsi::FleCSIGetHandle c(sema,macro.token);
      c.client_handle = macro.str(sema,pos++);
      c.nspace        = macro.str(sema,pos++);
      c.name          = macro.str(sema,pos++);
      c.data_type     = macro.str(sema,pos++);
      c.storage_class = macro.str(sema,pos++);
      c.version       = getUIntArg(ta,5); // 5 = template argument position
      prep.yaml().FleCSIGetHandle.push_back(c);
   }

   // flecsi_get_client_handle
   if (match(call, expr, name, "flecsi_get_client_handle",
      "flecsi::data::data_client_interface__", "get_client_handle")) {
      flecsi::FleCSIGetClientHandle c(sema,macro.token);
      c.client_type = macro.str(sema,pos++);
      c.nspace      = macro.str(sema,pos++);
      c.name        = macro.str(sema,pos++);
      prep.yaml().FleCSIGetClientHandle.push_back(c);
   }

   // flecsi_get_mutator
   if (match(call, expr, name, "flecsi_get_mutator",
      "flecsi::data::field_interface__", "get_mutator", 2)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(expr);
      flecsi::FleCSIGetMutator c(sema,macro.token);
      c.client_handle = macro.str(sema,pos++);
      c.nspace        = macro.str(sema,pos++);
      c.name          = macro.str(sema,pos++);
      c.data_type     = macro.str(sema,pos++);
      c.storage_class = macro.str(sema,pos++);
      c.version       = getUIntArg(ta,5); // 5 = template argument position
      pos++;  // version pulled via other means; skip to slots
      c.slots         = macro.str(sema,pos++);
      prep.yaml().FleCSIGetMutator.push_back(c);
   }

   // flecsi_get_global
   // this macro macros the flecsi_get_handle macro...
   if (match(call, expr, name, "flecsi_get_global",
      "flecsi::data::field_interface__", "get_handle", 1)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(expr);
      flecsi::FleCSIGetGlobal c(sema,macro.token);
      c.nspace    = macro.str(sema,pos++);
      c.name      = macro.str(sema,pos++);
      c.data_type = macro.str(sema,pos++);
      c.version   = getUIntArg(ta,5); // 5 = template argument position
      prep.yaml().FleCSIGetGlobal.push_back(c);
   }

   // flecsi_get_color
   // this macro macros the flecsi_get_handle macro...
   if (match(call, expr, name, "flecsi_get_color",
      "flecsi::data::field_interface__", "get_handle", 1)) {
      const clang::TemplateArgumentList *const ta = getTemplateArgs(expr);
      flecsi::FleCSIGetColor c(sema,macro.token);
      c.nspace    = macro.str(sema,pos++);
      c.name      = macro.str(sema,pos++);
      c.data_type = macro.str(sema,pos++);
      c.version   = getUIntArg(ta,5); // 5 = template argument position
      prep.yaml().FleCSIGetColor.push_back(c);
   }

   /*
   // apparently deprecated...

   // flecsi_get_handles
   if (match(call, expr, name, "flecsi_get_handles",
      "flecsi::data::field_interface__", "get_handles", 2)) {
      flecsi::FleCSIGetHandles c(sema,macro.token);
      c.client        = macro.str(sema,pos++);
      c.nspace        = macro.str(sema,pos++);
      c.data_type     = macro.str(sema,pos++);
      c.storage_class = macro.str(sema,pos++);
      c.version       = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs, 2);
      prep.yaml().FleCSIGetHandles.push_back(c);
   }

   // flecsi_get_handles_all
   if (match(call, expr, name, "flecsi_get_handles_all",
      "flecsi::data::field_interface__", "get_handles", 2)) {
      flecsi::FleCSIGetHandlesAll c(sema,macro.token);
      c.client        = macro.str(sema,pos++);
      c.data_type     = macro.str(sema,pos++);
      c.storage_class = macro.str(sema,pos++);
      c.version       = macro.str(sema,pos++);
      getVarArgs(expr, c.varargs, 2);
      prep.yaml().FleCSIGetHandlesAll.push_back(c);
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
VisitTypeAliasDecl(const clang::TypeAliasDecl *const tad)
{
   kitsune_debug("PreprocessorASTVisitor::VisitTypeAliasDecl()");
   kitsune_print(tad->getNameAsString());

   // associated macro invocation (if any); token name
   const MacroInvocation *const iptr = prep.get_invocation(tad->getLocStart());
   if (!iptr) return true;
   const MacroInvocation &macro = *iptr;
   const std::string name = tostr(sema,macro.token);

   // for use below
   std::size_t pos = 0;


   // ------------------------
   // Function
   // interface
   // ------------------------

   // flecsi_define_function_type(func, return_type, ...)
   if (name == "flecsi_define_function_type") {
      flecsi::FleCSIDefineFunctionType c(sema,macro.token);
      c.func        = macro.str(sema,pos++);
      c.return_type = macro.str(sema,pos++);
      getVarArgsFleCSIFunctionHandle(tad, c.varargs);
      prep.yaml().FleCSIDefineFunctionType.push_back(c);
   }

   // flecsi_register_function(func, nspace)
   // See VisitVarDecl()

   // flecsi_function_handle(func, nspace)
   // See VisitCallExpr()

   // flecsi_execute_function(handle, ...)
   // See VisitCallExpr()

   return true;
}

}
