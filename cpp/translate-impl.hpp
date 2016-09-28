#include "clang/Frontend/ASTConsumers.h"
#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"


enum class TypeMode { param, var, str };
std::string TranslateQualType(clang::QualType qt, TypeMode mode = TypeMode::var);

void TranslateDecl(const clang::Decl *d);
