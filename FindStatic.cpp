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
	std::set<std::string> ParsedTemplates;

public:
	CompilerInstance &Instance;

	PrintFunctionsConsumer(CompilerInstance &Instance,
			       std::set<std::string> ParsedTemplates)
		: ParsedTemplates(ParsedTemplates)
		, Instance(Instance)
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
