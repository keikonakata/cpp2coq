#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "translate.hpp"

bool IsCallByLocation(QualType qt);

class translateDecl : public Translate
{
    void TranslateFunctionDecl(const FunctionDecl *d) override;
    void TranslateVarDecl(const VarDecl *d) override;
    void TranslateFieldDecl(const FieldDecl *d) override;

public:
    translateDecl(ASTContext &cxt);
};
