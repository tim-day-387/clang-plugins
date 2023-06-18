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
class PrintFunctionsConsumer : public ASTConsumer {
	CompilerInstance &Instance;
	std::set<std::string> ParsedTemplates;

public:
	PrintFunctionsConsumer(CompilerInstance &Instance,
			       std::set<std::string> ParsedTemplates)
		: Instance(Instance)
		, ParsedTemplates(ParsedTemplates)
	{
	}

	bool HandleTopLevelDecl(DeclGroupRef DG) override
	{
		for (DeclGroupRef::iterator i = DG.begin(), e = DG.end();
		     i != e; ++i) {
			const Decl *D = *i;

			if (const NamedDecl *ND = dyn_cast<NamedDecl>(D))
				llvm::errs() << "top-level-decl: "
					     << ND->getNameAsString()
					     << ", source-loc: "
					     << D->getLocation().printToString(D->getASTContext().getSourceManager())
					     << ", in-main-file: "
					     << D->getASTContext().getSourceManager().isInMainFile(D->getLocation())
					     << "\n";
		}

		return true;
	}

	void HandleTranslationUnit(ASTContext &context) override
	{
		if (!Instance.getLangOpts().DelayedTemplateParsing)
			return;

		/*
		 * This demonstrates how to force instantiation of some templates in
		 * -fdelayed-template-parsing mode. (Note: Doing this unconditionally for
		 * all templates is similar to not using -fdelayed-template-parsig in the
		 * first place.)
		 * The advantage of doing this in HandleTranslationUnit() is that all
		 * codegen (when using -add-plugin) is completely finished and this can't
		 * affect the compiler output.
		 */
		struct Visitor : public RecursiveASTVisitor<Visitor> {
			const std::set<std::string> &ParsedTemplates;

			Visitor(const std::set<std::string> &ParsedTemplates)
				: ParsedTemplates(ParsedTemplates)
			{
			}

			bool VisitFunctionDecl(FunctionDecl *FD)
			{
				if (FD->isLateTemplateParsed() &&
				    ParsedTemplates.count(
					    FD->getNameAsString()))
					LateParsedDecls.insert(FD);
				return true;
			}

			std::set<FunctionDecl *> LateParsedDecls;
		} v(ParsedTemplates);

		v.TraverseDecl(context.getTranslationUnitDecl());

		clang::Sema &sema = Instance.getSema();

		for (const FunctionDecl *FD : v.LateParsedDecls) {
			clang::LateParsedTemplate &LPT =
				*sema.LateParsedTemplateMap.find(FD)->second;
			sema.LateTemplateParser(sema.OpaqueParser, LPT);
			llvm::errs() << "late-parsed-decl: \""
				     << FD->getNameAsString() << "\"\n";
		}
	}
};

class FindStaticAction : public PluginASTAction {
	std::set<std::string> ParsedTemplates;

protected:
	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
						       llvm::StringRef) override
	{
		return std::make_unique<PrintFunctionsConsumer>(
			CI, ParsedTemplates);
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
