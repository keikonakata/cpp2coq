#include "translate-decl.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateDeclOptions("translate-decl options");

void Translate::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "Parameter " << vname << " :=\n";
}

void Translate::TranslateFunctionDecl(const FunctionDecl *d) {
    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    if (!d->hasBody()) {
        outs() << "(* " << fname << " has no body. *)\n";
    }

    outs() << "Parameter " << fname;;

    if (d->param_empty()) {
        outs() << " (_ : value void)";
    } else {
        for (auto param : d->parameters()) {
            std::string pname = param->getNameAsString();

            QualType qt_param = param->getType();
            std::string sname = TranslateQualType(qt_param, TypeMode::var);

            outs() << " (" << pname << " : value " << sname << ")";

        }
    }

    QualType qt_ret = d->getReturnType();
    std::string str_ret = TranslateQualType(qt_ret, TypeMode::var);
    outs() << " : result " << str_ret << ".\n";

}

class TranslateDeclConsumer : public ASTConsumer {

public:

    explicit TranslateDeclConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateDecl().TranslateDeclContext(context.getTranslationUnitDecl());
    }

private:
    StringRef fname;
    CompilerInstance &compiler;

};

class TranslateDeclAction : public ASTFrontendAction {
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef file) {
        return std::unique_ptr<ASTConsumer>(new TranslateDeclConsumer(Compiler, file));
    }

};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, TranslateDeclOptions);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    int result = tool.run(newFrontendActionFactory<TranslateDeclAction>().get());

    return result;
}
