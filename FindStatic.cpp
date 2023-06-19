// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//
// Clang plugin to find functions which could be made
// static.
//
// Author: Timothy Day <tday141@gmail.com>
//

#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Sema/Sema.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;

namespace {

static std::unordered_map<std::string, int> functionMap;

class FunctionDeclVisitor : public RecursiveASTVisitor<FunctionDeclVisitor> {
  DiagnosticsEngine &Diags;
  unsigned WarningFoundStatic;

public:
  FunctionDeclVisitor(DiagnosticsEngine &Diags) : Diags(Diags) {
    WarningFoundStatic = Diags.getCustomDiagID(
        DiagnosticsEngine::Warning, "Should this function be static?");
  }

  bool VisitFunctionDecl(FunctionDecl *MethodDecl) {
    auto isValidDef =
        MethodDecl->isThisDeclarationADefinition() && MethodDecl->hasBody();
    auto isNotDeclared =
        (functionMap.count(MethodDecl->getNameAsString()) == 0);
    auto isNotStatic = !MethodDecl->isStatic();
    auto couldBeStatic =
        MethodDecl->getASTContext().getSourceManager().isInMainFile(
            MethodDecl->getLocation()) &&
        !MethodDecl->isMain();

    if (isValidDef && isNotDeclared && isNotStatic && couldBeStatic) {
      Diags.Report(MethodDecl->getLocation(), WarningFoundStatic);
    } else {
      functionMap.insert({MethodDecl->getNameAsString(), 1});
    }

    return true;
  }
};

class PrintFunctionsConsumer : public ASTConsumer {
public:
  void HandleTranslationUnit(ASTContext &Context) override {
    FunctionDeclVisitor Visitor(Context.getDiagnostics());
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class FindStaticAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;

protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<PrintFunctionsConsumer>();
  }

  /// Consume plugin arguments; the plugin has no args, so always
  /// succeed.
  ///
  /// \returns true when the parsing succeeds
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  /// Determines when to run action, in this case, automatically
  /// after the main AST action.
  ///
  /// \returns when the action should run
  PluginASTAction::ActionType getActionType() override {
    return AddAfterMainAction;
  }
};

} // namespace

static FrontendPluginRegistry::Add<FindStaticAction>
    X("find-static", "find potential static functions");
