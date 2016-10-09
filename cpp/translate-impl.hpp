#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "translate.hpp"

enum class StmtMode { semicolon, period, none };

bool IsAlias(QualType qty);
bool IsAlias(const Type *ty);

class translateImpl : public Translate
{
    void TranslateDeclStmt(Decl *d);
    void TranslateStmt(Stmt *s, StmtMode mode = StmtMode::semicolon);
    std::string TranslateExpr(Expr *e, std::string name = "");
    void virtual TranslateFunctionDecl(const FunctionDecl *d);
    void virtual TranslateVarDecl(const VarDecl *d);

public:
    translateImpl(ASTContext &cxt);
};
