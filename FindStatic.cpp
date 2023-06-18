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

class FunctionDeclVisitor : public RecursiveASTVisitor<FunctionDeclVisitor> {
	DiagnosticsEngine &Diags;
	unsigned WarningUnpropagatedEarlyExit;
	unsigned BigWarning;

public:
	FunctionDeclVisitor(DiagnosticsEngine &Diags)
		: Diags(Diags)
	{
		WarningUnpropagatedEarlyExit = Diags.getCustomDiagID(
			DiagnosticsEngine::Warning, "Has definition: %q0");
		BigWarning = Diags.getCustomDiagID(
			DiagnosticsEngine::Warning,
			"Doesn't have definition: %q0");
	}

	bool VisitFunctionDecl(FunctionDecl *MethodDecl)
	{
		if (MethodDecl->isThisDeclarationADefinition() &&
		    MethodDecl->hasBody()) {
			Diags.Report(MethodDecl->getLocation(),
				     WarningUnpropagatedEarlyExit)
			  << MethodDecl->getASTContext()
			   .getSourceManager()
			   .isInMainFile(MethodDecl->getLocation());
		} else {
			Diags.Report(MethodDecl->getLocation(), BigWarning)
			  << MethodDecl->getASTContext()
			  .getSourceManager()
			  .isInMainFile(MethodDecl->getLocation());
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
