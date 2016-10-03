#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "translate.hpp"

class translateQuirk : public Translate
{
    const clang::SourceManager *sm;

    void virtual TranslateFunctionDecl(const FunctionDecl *d);
    void virtual TranslateVarDecl(const VarDecl *d);

public:
    translateQuirk(const clang::SourceManager *sm);
};

