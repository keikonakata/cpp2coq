#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "translate.hpp"

enum class StmtMode { semicolon, period, none };

class translateImpl : public Translate
{
    void TranslateStmt(Stmt *s, StmtMode mode = StmtMode::semicolon);
    std::string TranslateExpr(Expr *e, std::string name = "");
    void virtual TranslateFunctionDecl(const FunctionDecl *d);
    void virtual TranslateVarDecl(const VarDecl *d);

public:
    translateImpl(const clang::SourceManager *sm);
};
