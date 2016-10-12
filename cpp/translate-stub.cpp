#include "translate-stub.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateStubOptions("translate-stub options");

void translateStub::TranslateRecordDecl(RecordDecl *d) {
    if (d->isInjectedClassName()) {
        return;
    }

    outs() << "\n";

    std::string rname = NameOfRecordDecl(d);
    outs() << "module " << rname << " = struct\n";

    for (auto decl : d->decls()) {
        TranslateDecl(decl);
    }

    outs() << "\n";
    outs() << "end\n";
}


void translateStub::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "let " << vname << " = \n";
}

void translateStub::TranslateFunctionDecl(const FunctionDecl *d) {
    if (!d->hasBody()) return;

    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    outs() << "let " << fname << " : (";

    if (CXXMethodDecl::classof(d)) {
        CXXMethodDecl *cxxmdecl = (CXXMethodDecl *) d;
        if (cxxmdecl->isInstance()) {
            outs() << "value -> ";
        }
    }

    for (auto param : d->parameters()) {
        outs() << "value -> ";
    }

    outs() << "m) ref = ref (fun _ -> assert false)\n";

}

translateStub::translateStub(ASTContext &cxt) : Translate(cxt) {}

class TranslateStubConsumer : public ASTConsumer {

public:

    explicit TranslateStubConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateStub(context).TranslateTranslationUnitDecl(context.getTranslationUnitDecl());
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

    outs() << "open Values\n";
    outs() << "open Compt\n";

    int result = tool.run(newFrontendActionFactory<TranslateStubAction>().get());

    return result;
}
