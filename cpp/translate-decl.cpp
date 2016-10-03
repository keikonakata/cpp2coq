#include "translate-decl.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateDeclOptions("translate-decl options");

void translateDecl::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "Parameter " << vname << " :=\n";
}

void translateDecl::TranslateFunctionDecl(const FunctionDecl *d) {
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
    outs() << " : fanswer " << str_ret << ".\n";

    std::string name = "name";
    //std::string fname = ToStubFile(NameOfFile(d)) + "." + NameOfPath(d) + "." + NameOfDecl(d);
    outs() << "Extract Constant " << fname << " => \"";
    if (d->param_empty()) {
        outs() << "fun (v : value) => fun (st : state) => ";
        outs() << "call_ (" << name << " v) st\n";
    } else {
        for (auto param : d->parameters()) {
            outs() << "fun (" << param->getNameAsString() << " : value) => ";
        }

        outs() << "fun (st : state) => ";
        for (auto param : d->parameters()) {
            std::string pname = param->getNameAsString();
            QualType qt_param = param->getType();

            if (1) {
                std::string sname = TranslateQualType(qt_param, TypeMode::var);

                outs() << "let " << pname << " = alloc_with_value " << sname << " " << pname << " in\n";
            }

        }
        outs() << "call_ (" << name;
        for (auto param : d-> parameters()) {
            outs() << " " << param->getNameAsString();
        }
        outs() << ")" << " st\n";
    }

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
