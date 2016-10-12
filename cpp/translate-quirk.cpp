#include "translate-quirk.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateQuirkOptions("translate-quirk options");

void translateQuirk::TranslateRecordDecl(RecordDecl *d) {
    if (d->isInjectedClassName())  return;

    for (auto decl : d->decls()) {
        TranslateDecl(decl);
    }
}

void translateQuirk::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "(* let " << vname << " = *)\n";
}

void translateQuirk::TranslateFunctionDecl(const FunctionDecl *d) {
    if (!d->hasBody()) return;

    outs() << "\n";

    std::string fname_stub = StubFile(FileOfDecl(d, _cxt.getSourceManager()));
    std::string fname_impl = ImplFile(FileOfDecl(d, _cxt.getSourceManager()));
    std::string name = PathOfFunctionDecl(d);

    std::string sname = fname_impl + "." + name;
    std::string tname = fname_stub + "." + name;

    outs() << "let _ = " << tname << " := " << sname << ";;\n";;

}

translateQuirk::translateQuirk(ASTContext &cxt) : Translate(cxt){}

class TranslateQuirkConsumer : public ASTConsumer {

public:

    explicit TranslateQuirkConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateQuirk(context).TranslateTranslationUnitDecl(context.getTranslationUnitDecl());
    }

private:
    StringRef fname;
    CompilerInstance &compiler;

};

class TranslateQuirkAction : public ASTFrontendAction {
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef file) {
        return std::unique_ptr<ASTConsumer>(new TranslateQuirkConsumer(Compiler, file));
    }

};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, TranslateQuirkOptions);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    int result = tool.run(newFrontendActionFactory<TranslateQuirkAction>().get());

    return result;
}
