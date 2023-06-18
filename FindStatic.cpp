// SPDX-License-Identifier: GPL-2.0

/*
 * Clang plugin to find functions which could be made
 * static.
 *
 * Author: Timothy Day <tday141@gmail.com>
 *
 */

#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Sema/Sema.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;

namespace
{

static std::unordered_map<std::string, int> functionMap;

class FunctionDeclVisitor : public RecursiveASTVisitor<FunctionDeclVisitor> {
	DiagnosticsEngine &Diags;
	unsigned WarningFoundStatic;

public:
	FunctionDeclVisitor(DiagnosticsEngine &Diags)
		: Diags(Diags)
	{
		WarningFoundStatic = Diags.getCustomDiagID(
			DiagnosticsEngine::Warning, "Should this function be static?");
	}

	bool VisitFunctionDecl(FunctionDecl *MethodDecl)
	{
		if (MethodDecl->isThisDeclarationADefinition() &&
		    MethodDecl->hasBody() &&
		    functionMap.count(MethodDecl->getNameAsString()) == 0 &&
		    !MethodDecl->isStatic() &&
		    MethodDecl->getASTContext().getSourceManager()
		    .isInMainFile(MethodDecl->getLocation()) &&
		    !MethodDecl->isMain()) {
			Diags.Report(MethodDecl->getLocation(),
				     WarningFoundStatic);
		} else {
			functionMap.insert({MethodDecl->getNameAsString(), 1});
		}

		return true;
	}
};

class PrintFunctionsConsumer : public ASTConsumer {
public:
	void HandleTranslationUnit(ASTContext &Context) override
	{
		FunctionDeclVisitor Visitor(Context.getDiagnostics());
		Visitor.TraverseDecl(Context.getTranslationUnitDecl());
	}
};

class FindStaticAction : public PluginASTAction {
	std::set<std::string> ParsedTemplates;

protected:
	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
						       llvm::StringRef) override
	{
		return std::make_unique<PrintFunctionsConsumer>();
	}

	/**
	 * ParseArgs() - Consume plugin arguments.
	 *
	 * The plugin has no args, so always succeed.
	 *
	 * Return: Has the parsing succeeded?
	 */
	bool ParseArgs(const CompilerInstance &CI,
		       const std::vector<std::string> &args) override
	{
		return true;
	}

	/**
	 * getActionType() - Determine when to run action.
	 *
	 * Automatically run the plugin after the main AST
	 * action.
	 *
	 * Return: When the action should run.
	 */
	PluginASTAction::ActionType getActionType() override
	{
		return AddAfterMainAction;
	}
};

}

/*
 * Make Clang aware of the find-static plugin.
 */
static FrontendPluginRegistry::Add<FindStaticAction>
	X("find-static", "find potential static functions");
