#include "translate-quirk.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateQuirkOptions("translate-quirk options");

void translateQuirk::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "(* let " << vname << " = *)\n";
}

std::string NameOfPath(const FunctionDecl *d) {
    return std::string { "" };
}

std::string NameOfFile(const FunctionDecl *d, const SourceManager *sm) {

    FunctionTemplateDecl *ftdecl = d->getPrimaryTemplate();
    if (ftdecl) {
        return std::string { "NameOfFile::ftdecl is not null" };
    }

    SourceLocation sloc = d->getOuterLocStart();
    return sm->getFilename(sloc);
}


void translateQuirk::TranslateFunctionDecl(const FunctionDecl *d) {
    outs() << "\n";

    outs() << NameOfFile(d, sm) << "\n";

    // std::string fname_stub = ToStubFile(NameOfFile(d));
    // std::string fname_impl = ToImpleFile(NameOfFile(d));
    // std::string name = NameOfPath(d) + "." + NameOfDecl(d);

    // std::string sname = fname_stub + "." + name;
    // std::string tname = fname_impl + "." + name;

    // if (!d->hasBody()) {
    //     outs() << "(* " << name << " has no body. *)\n";
    // }

    // outs() << "let _ = " << tname << " := " << sname << ";;\n";;

}

translateQuirk::translateQuirk(const SourceManager *sm_) : sm (sm_){}

class TranslateQuirkConsumer : public ASTConsumer {

public:

    explicit TranslateQuirkConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateQuirk(&context.getSourceManager()).TranslateDeclContext(context.getTranslationUnitDecl());
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
