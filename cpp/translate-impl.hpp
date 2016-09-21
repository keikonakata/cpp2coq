#include "clang/Frontend/ASTConsumers.h"
#include "clang/AST/Expr.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"


enum class TypeMode { var, str };
std::string TranslateQualType(clang::QualType qt, TypeMode mode = TypeMode::var);
