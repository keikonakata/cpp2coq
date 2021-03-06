#include "clang/AST/AST.h"

using namespace llvm;
using namespace clang;


std::string PathOfDeclContext(const DeclContext *dcxt, std::string sep = ".");

// concatenate 'v', separated by 's'
std::string ConcatVector(std::vector<std::string> v, std::string s = "");
// concatenate 'v', separated and prefixed by 's'
std::string ConcatVector2(std::vector<std::string> v, std::string s = "");

enum class TypeMode { param, var, str };

std::string PathOfFunctionDecl(const FunctionDecl *fdecl);

std::string FileOfDecl(const FieldDecl *d, const SourceManager &sm);
std::string FileOfDecl(const FunctionDecl *d, const SourceManager &sm);

std::string DeclFile(std::string s);
std::string ImplFile(std::string s);
std::string StubFile(std::string s);

void TranslateTemplateArgument(const TemplateArgument &arg, std::vector<std::string> &names, TypeMode mode);

std::string TranslateQualType(const QualType qt, TypeMode mode);

std::string TranslateType(const Type *ty, TypeMode mode);

std::string TypeOfRecordDecl(const RecordDecl *rdecl);

std::string TranslateBuiltinType(const BuiltinType *ty);

std::string NameOfFunctionDecl(const FunctionDecl *d);

std::string NameOfRecordDecl(const RecordDecl *d);

class Translate {

    void virtual TranslateFieldDecl(const FieldDecl *d);

    void virtual TranslateFunctionDecl(const FunctionDecl *d);

    void virtual TranslateVarDecl(const VarDecl *d);

    void virtual TranslateRecordDecl(RecordDecl *d);

    void virtual TranslateEnumDecl(EnumDecl *d);

    void TranslateNamespaceDecl(const NamespaceDecl *d);

    void TranslateFunctionTemplateDecl(FunctionTemplateDecl *d);

protected:
    ASTContext &_cxt;
    void TranslateDecl(const Decl *d);

public:
    void TranslateTranslationUnitDecl(const TranslationUnitDecl *dc);
    Translate(ASTContext &cxt);

};
