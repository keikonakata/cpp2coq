#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "translate.hpp"

class translateQuirk : public Translate
{
    void virtual TranslateFunctionDecl(const FunctionDecl *d);
    void virtual TranslateVarDecl(const VarDecl *d);
    void virtual TranslateRecordDecl(RecordDecl *d);

public:
    translateQuirk(ASTContext &cxt);
};

