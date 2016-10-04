#include "clang/AST/AST.h"

using namespace llvm;
using namespace clang;


std::string ConcatVector(std::vector<std::string> v, std::string s = "");

enum class TypeMode { param, var, str };

std::string NameOfPath(const FunctionDecl *d);

std::string NameOfFile(const FunctionDecl *d, const SourceManager *sm);

std::string StubFile(std::string s);

std::string ImplFile(std::string s);

std::string TranslateQualType(const QualType qt, TypeMode mode);

std::string TranslateType(const Type *ty, TypeMode mode);

std::string TranslateBuiltinType(const BuiltinType *ty);

std::string NameOfFunctionDecl(const FunctionDecl *d);

std::string NameOfRecordDecl(const RecordDecl *d);

class Translate {

    void virtual TranslateFunctionDecl(const FunctionDecl *d);

    void virtual TranslateVarDecl(const VarDecl *d);

    void TranslateRecordDecl(RecordDecl *d);

    void TranslateFunctionTemplateDecl(FunctionTemplateDecl *d);

    void TranslateDecl(const Decl *d);

public:
    void TranslateDeclContext(const DeclContext *dc);

};
