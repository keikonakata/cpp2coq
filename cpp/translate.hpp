#include "clang/AST/AST.h"

using namespace llvm;
using namespace clang;


std::string PathOfDeclContext(const DeclContext *dcxt, std::string sep = ".");

std::string ConcatVector(std::vector<std::string> v, std::string s = "");

enum class TypeMode { param, var, str };

std::string PathOfFunctionDecl(const FunctionDecl *fdecl);

std::string FileOfFunctionDecl(const FunctionDecl *d, const SourceManager *sm);

std::string DeclFile(std::string s);
std::string ImplFile(std::string s);
std::string StubFile(std::string s);

std::string TranslateQualType(const QualType qt, TypeMode mode);

std::string TranslateType(const Type *ty, TypeMode mode);

std::string TranslateBuiltinType(const BuiltinType *ty);

std::string NameOfFunctionDecl(const FunctionDecl *d);

std::string NameOfRecordDecl(const RecordDecl *d);

class Translate {

    void virtual TranslateFunctionDecl(const FunctionDecl *d);

    void virtual TranslateVarDecl(const VarDecl *d);

    void virtual TranslateRecordDecl(RecordDecl *d);

    void TranslateFunctionTemplateDecl(FunctionTemplateDecl *d);

    void TranslateDecl(const Decl *d);

protected:
    const SourceManager *sm;

public:
    void TranslateDeclContext(const DeclContext *dc);
    Translate(const SourceManager *sm);

};
