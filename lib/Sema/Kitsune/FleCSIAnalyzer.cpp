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

#include "clang/Sema/Kitsune/FleCSIAnalyzer.h"

#include <iostream>

#include "llvm/Support/YAMLTraits.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Sema.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Sema/SemaDiagnostic.h"

#define np(X)                                                            \
 std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ \
           << ": " << #X << " = " << (X) << std::endl

using namespace std;
using namespace llvm;
using namespace yaml;
using namespace clang;
using namespace sema;

using llvm::yaml::MappingTraits;
using llvm::yaml::IO;
using llvm::yaml::Output;

namespace llvm{
namespace yaml{

struct RegisterDataClient{
  string type = "register_data_client";
  string file;
  uint32_t line;
  string meshType;
  string nspace;
  string name;
};

template <>
struct MappingTraits<RegisterDataClient> {
  static void mapping(IO& io, RegisterDataClient& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("meshType", c.meshType);
    io.mapRequired("nspace", c.nspace);
    io.mapRequired("name", c.name);
  }
};

struct RegisterTask{
  string type = "register_task";
  string file;
  uint32_t line;
  string name;
  string processor;
  string launch;
};

template <>
struct MappingTraits<RegisterTask> {
  static void mapping(IO& io, RegisterTask& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("name", c.name);
    io.mapRequired("processor", c.processor);
    io.mapRequired("launch", c.launch);
  }
};

struct RegisterField{
  string type = "register_field";
  string file;
  uint32_t line;
  string meshType;
  string nspace;
  string name;
  string dataType;
  string storageType;
  uint32_t versions;
  uint32_t indexSpace;
};

template <>
struct MappingTraits<RegisterField> {
  static void mapping(IO& io, RegisterField& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("meshType", c.meshType);
    io.mapRequired("nspace", c.nspace);
    io.mapRequired("name", c.name);
    io.mapRequired("dataType", c.dataType);
    io.mapRequired("storageType", c.storageType);
    io.mapRequired("versions", c.versions);
    io.mapRequired("indexSpace", c.indexSpace);
  }
};

struct ExecuteTask{
  string type = "execute_task";
  string file;
  uint32_t line;
  string name;
  string launch;
};

template <>
struct MappingTraits<ExecuteTask> {
  static void mapping(IO& io, ExecuteTask& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("name", c.name);
    io.mapRequired("launch", c.launch);
  }
};

struct ExecuteMPITask{
  string type = "execute_mpi_task";
  string file;
  uint32_t line;
  string name;
};

template <>
struct MappingTraits<ExecuteMPITask> {
  static void mapping(IO& io, ExecuteMPITask& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("name", c.name);
  }
};

struct GetHandle{
  string type = "get_handle";
  string file;
  uint32_t line;
  string nspace;
  string name;
};

template <>
struct MappingTraits<GetHandle> {
  static void mapping(IO& io, GetHandle& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("nspace", c.nspace);
    io.mapRequired("name", c.name);
  }
};

struct GetClientHandle{
  string type = "get_client_handle";
  string file;
  uint32_t line;
  string meshType;
  string nspace;
  string name;
};

template <>
struct MappingTraits<GetClientHandle> {
  static void mapping(IO& io, GetClientHandle& c) {
    io.mapRequired("type", c.type);
    io.mapRequired("file", c.file);
    io.mapRequired("line", c.line);
    io.mapRequired("meshType", c.meshType);
    io.mapRequired("nspace", c.nspace);
    io.mapRequired("name", c.name);
  }
};

template <class T>
struct SequenceTraits<std::vector<T>> {
  static size_t size(IO& io, std::vector<T>& vec) {
    return vec.size();
  }

  static T& element(IO& io, std::vector<T>& vec, size_t index) {
    return vec[index];
  }
};

struct FleCSIMetadata{
  vector<RegisterDataClient> dataClientRegs;
  vector<RegisterTask> taskRegs;
  vector<RegisterField> fieldRegs;
  vector<ExecuteTask> taskExecs;
  vector<ExecuteMPITask> mpiTaskExecs;
  vector<GetHandle> handleGets;
  vector<GetClientHandle> clientHandleGets;
};

template <>
struct MappingTraits<FleCSIMetadata> {
  static void mapping(IO& io, FleCSIMetadata& m) {
    io.mapRequired("dataClientRegs", m.dataClientRegs);
    io.mapRequired("taskRegs", m.taskRegs);
    io.mapRequired("fieldRegs", m.fieldRegs);
    io.mapRequired("taskExecs", m.taskExecs);
    io.mapRequired("mpiTaskExecs", m.mpiTaskExecs);
    io.mapRequired("handleGets", m.handleGets);
    io.mapRequired("clientHandleGets", m.clientHandleGets);
  }
};

} // namespace yaml
} // namespace llvm

namespace{

  string str(const clang::Token& token){
    return token.getIdentifierInfo()->getName().str();
  }

  FleCSIAnalyzer* _flecsi_analyzer = nullptr;

  struct MacroUse{
    clang::Token tok;
    vector<clang::Token> args;

    string arg(size_t i) const{
      return str(args[i]);
    }

    SourceLocation loc(size_t i) const{
      return args[i].getLocation();
    }
  };

  class PreprocessorAnalyzer : public PPCallbacks{
  public:

    PreprocessorAnalyzer(Sema& sema)
    : sema_(sema),
    sourceMgr_(sema_.getSourceManager()){
      
      flecsiMacros_ = 
      {"flecsi_register_data_client",
       "flecsi_register_task",
       "flecsi_register_field",
       "flecsi_execute_task",
       "flecsi_execute_mpi_task",
       "flecsi_get_handle",
       "flecsi_get_client_handle"};
    }

    ~PreprocessorAnalyzer(){}

    void MacroDefined(const clang::Token& tok, const MacroDirective* md) override{}

    void MacroExpands(const clang::Token& tok,
                      const MacroDefinition& md,
                      SourceRange range,
                      const MacroArgs* args) override{
      
      IdentifierInfo* ii = tok.getIdentifierInfo();
      SourceLocation loc = tok.getLocation();

      if(flecsiMacros_.find(ii->getName().str()) != flecsiMacros_.end()){
        MacroUse use;
        use.tok = tok;

        size_t n = args->getNumMacroArguments();

        for(size_t i = 0; i < n; ++i){
          use.args.push_back(*args->getUnexpArgument(i));
        }

        auto p = sourceMgr_.getDecomposedExpansionLoc(tok.getLocation());
        
        sourceMap_[p.first][p.second] = use;
      }
    }

    MacroUse* getMacroUse(SourceLocation loc){
      auto p = sourceMgr_.getDecomposedExpansionLoc(loc);
      auto itr = sourceMap_.find(p.first);
      if(itr == sourceMap_.end()){
        return nullptr;
      }

      MacroUseMap& um = itr->second;
      auto mitr = um.lower_bound(p.second);
      if(mitr == um.end()){
        return nullptr;
      }

      MacroUse& mu = mitr->second;
      if(p.second > mitr->first + mu.tok.getLength()){
        return nullptr;
      }

      return &mitr->second;
    }

    FleCSIMetadata& metadata(){
      return metadata_;
    }

  private:
    Sema& sema_;
    SourceManager& sourceMgr_;

    using MacroUseMap = map<size_t, MacroUse>; 

    using SourceMap = map<FileID, MacroUseMap>;

    SourceMap sourceMap_;

    set<string> flecsiMacros_;

    FleCSIMetadata metadata_;
  }; // class PreprocessorAnalyzer

  class Analyzer : public RecursiveASTVisitor<Analyzer>{
  public:
    Analyzer(Sema& sema, PreprocessorAnalyzer* pa)
    : sema_(sema),
    pa_(pa),
    md_(pa_->metadata()){}

    uint32_t getFileLine(MacroUse* mu, string& file){
      SourceManager& srcMgr = sema_.getSourceManager();
      SourceLocation loc = mu->tok.getLocation();
      auto p = srcMgr.getDecomposedLoc(loc);
      file = srcMgr.getFilename(loc).str();
      return srcMgr.getLineNumber(p.first, p.second);
    }

    bool VisitCallExpr(CallExpr* ce){
      Decl* cd = ce->getCalleeDecl();
      
      if(!cd){
        return true;
      }

      auto fd = dyn_cast<FunctionDecl>(cd);

      if(fd){
        DeclarationName dn = fd->getDeclName();

        if(!dn){
          return true;
        }

        if(!dn.isIdentifier()){
          return true;
        }

        auto mu = pa_->getMacroUse(ce->getLocStart());

        if(!mu){
          return true;
        }

        string name = str(mu->tok);

        if(name == "flecsi_execute_task"){
          const CXXRecordDecl* rd;
          if(getClassCall(ce,
            "flecsi::execution::task_model__",
            "execute_task", 3, rd)){
            ExecuteTask c;
            c.line = getFileLine(mu, c.file);
            c.name = mu->arg(0);
            c.launch = mu->arg(1);
            md_.taskExecs.push_back(c);            
          }
        }
        else if(name == "flecsi_execute_mpi_task"){
          const CXXRecordDecl* rd;
          if(getClassCall(ce,
            "flecsi::execution::task_model__",
            "execute_task", 3, rd)){
            ExecuteMPITask c;
            c.line = getFileLine(mu, c.file);
            c.name = mu->arg(0);
            md_.mpiTaskExecs.push_back(c);
          }
        }
        else if(name == "flecsi_get_handle"){
          const CXXRecordDecl* rd;
          if(getClassCall(ce,
            "flecsi::data::field_data__",
            "get_handle", 1, rd)){
            GetHandle c;
            c.line = getFileLine(mu, c.file);
            c.nspace = mu->arg(1);
            c.name = mu->arg(2);
            md_.handleGets.push_back(c);
          }
        }
        else if(name == "flecsi_get_client_handle"){
          const CXXRecordDecl* rd;
          if(getClassCall(ce,
            "flecsi::data::client_data__",
            "get_client_handle", 0, rd)){
            
            const CXXMethodDecl* md = getMethod(ce);

            const TemplateArgumentList* templateArgs = getTemplateArgs(md);

            QualType qt = getTypeArg(templateArgs, 0);

            const CXXRecordDecl* cd = getClassDecl(qt);

            if(!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")){
              sema_.Diag(mu->loc(0),
                diag::err_flecsi_not_a_data_client);
              return true;
            }

            GetClientHandle c;
            c.line = getFileLine(mu, c.file);
            c.meshType = mu->arg(0);
            c.nspace = mu->arg(1);
            c.name = mu->arg(2);
            md_.clientHandleGets.push_back(c);
          }
        }
      }

      return true;
    }

    bool VisitTranslationUnitDecl(TranslationUnitDecl* d){
      for(auto d : d->decls()){
        TraverseDecl(d);
      }

      return true;
    }

    const CallExpr* getClassCall(const Expr* e,
                                 const string& className,
                                 const string& call){
      const CXXRecordDecl* rd;
      return getClassCall(e, className, call,
        numeric_limits<int>::min(), numeric_limits<int>::max(), rd);
    }

    const CallExpr* getClassCall(const Expr* e,
                                 const string& className,
                                 const string& call,
                                 int numArgs){
      const CXXRecordDecl* rd;
      return getClassCall(e, className, call, numArgs, rd);
    }

    const CXXMethodDecl* getMethod(const CallExpr* ce){
      const Decl* cd = ce->getCalleeDecl();
      if(!cd){
        return nullptr;
      }

      auto md = dyn_cast<CXXMethodDecl>(cd);
      if(!md){
        return nullptr;
      }

      return md;      
    }

    const CallExpr* getClassCall(const Expr* e,
                                 const string& className,
                                 const string& call,
                                 int numArgs,
                                 const CXXRecordDecl*& rd){
      return getClassCall(e, className, call, numArgs, numArgs, rd);
    }     

    const CallExpr* getClassCall(const Expr* e,
                                 const string& className,
                                 const string& call,
                                 int minArgs,
                                 int maxArgs,
                                 const CXXRecordDecl*& rd){

      if(!e){
        return nullptr;
      }

      e = normExpr(e);

      auto ce = dyn_cast<CallExpr>(e);
      if(!ce){
        return nullptr;
      }

      const CXXMethodDecl* md = getMethod(ce);
      if(!md){
        return nullptr;
      }

      rd = md->getParent();

      int numArgs = ce->getNumArgs();

      if(numArgs < minArgs || numArgs > maxArgs){
        return nullptr;
      }

      return className == getQualifiedName(rd) && call == getName(md) ?
        ce : nullptr;
    }

    string getName(const NamedDecl* nd){
      DeclarationName dn = nd->getDeclName();
      if(!dn){
        return "";
      }

      if(!dn.isIdentifier()){
        return "";
      }

      return dn.getAsString();
    }

    string getQualifiedName(const NamedDecl* nd){
      return nd->getQualifiedNameAsString();
    }

    const Expr* normExpr(const Expr* e){
      if(auto ec = dyn_cast<ExprWithCleanups>(e)){
        return ec->getSubExpr();
      }

      return e;
    }

    const TemplateArgumentList* getTemplateArgs(const FunctionDecl* fd){
      return fd->getTemplateSpecializationArgs();
    }

    uint64_t getUIntArg(const TemplateArgumentList* args, size_t param){
      const TemplateArgument& arg = args->get(param);
      assert(arg.getKind() == TemplateArgument::Integral);
      return arg.getAsIntegral().getZExtValue();      
    }

    int64_t getIntArg(const TemplateArgumentList* args, size_t param){
      const TemplateArgument& arg = args->get(param);
      assert(arg.getKind() == TemplateArgument::Integral);
      return arg.getAsIntegral().getSExtValue();      
    }

    QualType getTypeArg(const TemplateArgumentList* args, size_t param){
      const TemplateArgument& arg = args->get(param);
      assert(arg.getKind() == TemplateArgument::Type);
      return arg.getAsType();      
    }

    const CXXRecordDecl* getClassDecl(QualType qt){
      const clang::Type* t = qt.getTypePtr();
      
      const RecordType* rt = dyn_cast<RecordType>(t);
      if(!rt){
        return nullptr;
      }

      const RecordDecl* rd = rt->getDecl();            
      const CXXRecordDecl* cd = dyn_cast<CXXRecordDecl>(rd);
      if(!cd){
        return nullptr;
      }

      return cd;  
    }

    bool isDerivedFrom(const CXXRecordDecl* cd,
                       const string& qualifiedBaseName){
      if(getQualifiedName(cd) == qualifiedBaseName){
        return true;
      }

      for(auto bi : cd->bases()){
        const CXXRecordDecl* bcd = 
          bi.getType().getTypePtr()->getAsCXXRecordDecl();

        if(bcd && isDerivedFrom(bcd, qualifiedBaseName)){
          return true;
        }
      }

      return false;
    }

    bool VisitVarDecl(VarDecl* vd){
      if(vd->getName().endswith("_registered")){
        auto mu = pa_->getMacroUse(vd->getLocStart());
        if(!mu){
          return true;
        }

        string name = str(mu->tok);

        if(name == "flecsi_register_task"){
          const CXXRecordDecl* rd;
          const CallExpr* ce = getClassCall(vd->getInit(),
            "flecsi::execution::task_model__",
            "register_task", 3, rd);

          if(ce){
            RegisterTask c;
            c.line = getFileLine(mu, c.file);
            c.name = mu->arg(0);
            c.processor = mu->arg(1);
            c.launch = mu->arg(2);
            md_.taskRegs.push_back(c);
          }
        }
        else if(name == "flecsi_register_field"){
          const CXXRecordDecl* rd;
          const CallExpr* ce = getClassCall(vd->getInit(),
            "flecsi::data::field_data__", "register_field", 1, rd);

          if(ce){
            const CXXMethodDecl* md = getMethod(ce);

            const TemplateArgumentList* templateArgs = getTemplateArgs(md);

            RegisterField c;
            c.line = getFileLine(mu, c.file);
            c.meshType = mu->arg(0);
            c.nspace = mu->arg(1);
            c.name = mu->arg(2);
            c.dataType = mu->arg(3);
            c.storageType = mu->arg(4);
            c.versions = getUIntArg(templateArgs, 5);
            c.indexSpace = getUIntArg(templateArgs, 6);
            md_.fieldRegs.push_back(c);            
          }
        }
        else if(name == "flecsi_register_data_client"){
          const CXXRecordDecl* rd;
          const CallExpr* ce = getClassCall(vd->getInit(),
            "flecsi::data::client_data__",
            "register_data_client", 1, rd);
          
          if(ce){
            const CXXMethodDecl* md = getMethod(ce);

            const TemplateArgumentList* templateArgs = getTemplateArgs(md);

            QualType qt = getTypeArg(templateArgs, 0);

            const CXXRecordDecl* cd = getClassDecl(qt);

            if(!cd || !isDerivedFrom(cd, "flecsi::data::data_client_t")){
              sema_.Diag(mu->loc(0),
                diag::err_flecsi_not_a_data_client);
              return true;
            }

            RegisterDataClient c;
            c.line = getFileLine(mu, c.file);
            c.meshType = mu->arg(0);
            c.nspace = mu->arg(1);
            c.name = mu->arg(2);
            md_.dataClientRegs.push_back(c);            
          }
        }
      }

      return true;
    }

  private:
    Sema& sema_;
    PreprocessorAnalyzer* pa_;
    FleCSIMetadata& md_;
  };

} // namespace

void FleCSIAnalyzer::init(Sema& sema){
  _flecsi_analyzer = new FleCSIAnalyzer(sema);
}

FleCSIAnalyzer* FleCSIAnalyzer::get(){
  assert(_flecsi_analyzer);
  return _flecsi_analyzer;
}

FleCSIAnalyzer::FleCSIAnalyzer(Sema& sema)
: sema_(sema){
  auto pa = new PreprocessorAnalyzer(sema);
  
  sema_.getPreprocessor().addPPCallbacks(
    unique_ptr<PreprocessorAnalyzer>(pa));
  pa_ = pa;
}

void FleCSIAnalyzer::gatherMetadata(Decl* decl){
  auto pa = static_cast<PreprocessorAnalyzer*>(pa_);
  Analyzer analyzer(sema_, pa);
  analyzer.TraverseDecl(decl);
}

void FleCSIAnalyzer::finalizeMetadata(CompilerInstance& ci){
  auto pa = static_cast<PreprocessorAnalyzer*>(pa_);
  FleCSIMetadata& md = pa->metadata();

  //yaml::Output out(llvm::outs());
  //out << md;
}
