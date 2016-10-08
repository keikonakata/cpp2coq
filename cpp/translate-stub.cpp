#include "translate-stub.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateStubOptions("translate-stub options");

void translateStub::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "let " << vname << " = \n";
}

void translateStub::TranslateFunctionDecl(const FunctionDecl *d) {
    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    if (!d->hasBody()) {
        outs() << "(* " << fname << " has no body. *)\n";
    }

    outs() << "let " << fname << " :";

    if (d->param_empty()) {
        outs() << "(value -> m) ref";
    } else {
        outs() << "(";
        for (auto param : d->parameters()) {
            outs() << "value -> ";
        }
        outs() << "m) ref";
    }

    outs() << " = ref (fun _ -> assert false)\n";;

}

translateStub::translateStub(const clang::SourceManager *sm_) : Translate(sm_) {}

class TranslateStubConsumer : public ASTConsumer {

public:

    explicit TranslateStubConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateStub(&context.getSourceManager()).TranslateDeclContext(context.getTranslationUnitDecl());
    }

private:
    StringRef fname;
    CompilerInstance &compiler;

};

class TranslateStubAction : public ASTFrontendAction {
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef file) {
        return std::unique_ptr<ASTConsumer>(new TranslateStubConsumer(Compiler, file));
    }

};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, TranslateStubOptions);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    int result = tool.run(newFrontendActionFactory<TranslateStubAction>().get());

    return result;
}
