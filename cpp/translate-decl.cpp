#include "translate-decl.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateDeclOptions("translate-decl options");


bool IsCallByLocation(const Type *ty) {
    if (BuiltinType::classof(ty)) {
        return true;
    } else if (PointerType::classof(ty)) {
        return true;
    } else if (ReferenceType::classof(ty)) {
        return false;
    } else if (TagType::classof(ty)) {
        if (RecordType::classof(ty)) {
            return true;
        } else {
            outs() << "IsCallByLocation::TagType::else\n";
            return false;
        }
    } else {
        outs() << "IsCallByLocation::else\n";
        return false;
    }

}

bool IsCallByLocation(QualType qt) {
    const Type *ty = qt.getTypePtrOrNull();
    if (ty != NULL) {
        return IsCallByLocation(ty);
    } else {
        outs() << "IsCallByLocation::NULL\n";
        return false;
    }
}


void translateDecl::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "Parameter " << vname << " :=\n";
}

void translateDecl::TranslateFunctionDecl(const FunctionDecl *d) {
    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    if (!d->hasBody()) {
        outs() << "(* " << fname << " has no body. *)\n";
        return;
    }

    outs() << "Parameter " << fname << " : forall";;

    if (CXXMethodDecl::classof(d)) {
        CXXMethodDecl *cxxmdecl = (CXXMethodDecl *) d;
        if (cxxmdecl->isInstance()) {
            QualType qt = cxxmdecl->getThisType(_cxt);
            outs() << " (this : value " << TranslateQualType(qt, TypeMode::var) << ")";
        }
    }

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
    outs() << ", fanswer " << str_ret << ".\n";

    std::string name_impl = StubFile(FileOfFunctionDecl(d, _cxt.getSourceManager())) + "." + PathOfFunctionDecl(d);
    outs() << "Extract Constant " << fname << " => \"";
    if (d->param_empty()) {
        outs() << "fun (v : value) -> fun (st : state) -> ";
        outs() << "call_ (" << name_impl << " v) st\n";
    } else {
        for (auto param : d->parameters()) {
            outs() << "fun (" << param->getNameAsString() << " : value) -> ";
        }

        outs() << "fun (st : state) -> ";
        for (auto param : d->parameters()) {
            std::string pname = param->getNameAsString();
            QualType qt_param = param->getType();

            if (IsCallByLocation(qt_param)) {
                std::string sname = TranslateQualType(qt_param, TypeMode::var);

                outs() << "let " << pname << " = salloc_with_value " << sname << " " << pname << " in\n";
            }

        }
        outs() << "call_ (" << name_impl;
        for (auto param : d-> parameters()) {
            outs() << " " << param->getNameAsString();
        }
        outs() << ")" << " st\".\n";
    }

}

translateDecl::translateDecl(ASTContext &cxt) : Translate(cxt) {}

class TranslateDeclConsumer : public ASTConsumer {

public:

    explicit TranslateDeclConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateDecl(context).TranslateDeclContext(context.getTranslationUnitDecl());
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
