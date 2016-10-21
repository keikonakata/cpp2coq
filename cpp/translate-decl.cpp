#include "translate-decl.hpp"

using namespace std;
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
    } else if (TypedefType::classof(ty)) {
        const TypedefType *tdty = (const TypedefType *) ty;
        return IsCallByLocation(tdty->desugar());
    } else if (SubstTemplateTypeParmType::classof(ty)) {
        const SubstTemplateTypeParmType * pty = (const SubstTemplateTypeParmType *) ty;
        return IsCallByLocation(pty->getReplacementType());
    } else {
        ty->dump();
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

void translateDecl::TranslateFieldDecl(const FieldDecl *d) {
    string fname = d->getNameAsString();
    const RecordDecl *rdecl = d->getParent();
    QualType qt = d->getType();

    outs() << "\n";

    outs() << "Parameter " << fname << " : value (Loc " << TypeOfRecordDecl(rdecl) << ") ->";

    outs() << " value " << TranslateQualType(qt, TypeMode::param) << ".\n";
    outs() << "Extract Constant " << fname << " => \"fun (this : value) ->\".\n";
}

void translateDecl::TranslateFunctionDecl(const FunctionDecl *d) {
    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    outs() << "Parameter " << fname << " : forall";

    if (CXXMethodDecl::classof(d)) {
        CXXMethodDecl *cxxmdecl = (CXXMethodDecl *) d;
        if (cxxmdecl->isInstance()) {
            QualType qt = cxxmdecl->getThisType(_cxt);
            outs() << " (this : value " << TranslateQualType(qt, TypeMode::var) << ")";
        }
    }

    vector<string> pnames;
    for (auto param : d->parameters()) {
        std::string pname = param->getNameAsString();
        if (pname == "") {
            pname = "tmp";
        }
        pnames.emplace_back(pname);

        QualType qt_param = param->getType();
        std::string sname = TranslateQualType(qt_param, TypeMode::var);

        outs() << " (" << pname << " : value " << sname << ")";

    }

    QualType qt_ret = d->getReturnType();
    std::string str_ret = TranslateQualType(qt_ret, TypeMode::var);
    outs() << ", fanswer " << str_ret << ".\n";

    std::string name_impl = StubFile(FileOfDecl(d, _cxt.getSourceManager())) + "." + PathOfFunctionDecl(d);
    outs() << "Extract Constant " << fname << " => \"";

    if (CXXMethodDecl::classof(d)) {
        CXXMethodDecl *cxxmdecl = (CXXMethodDecl *) d;
        if (cxxmdecl->isInstance()) {
            QualType qt = cxxmdecl->getThisType(_cxt);
            outs() << "fun (this : value) -> ";
        }
    }

    for (auto pname : pnames) {
        outs() << "fun (" << pname << " : value) -> ";
    }

    outs() << "fun (st : state) -> ";
    for (auto param : d->parameters()) {
        std::string pname = param->getNameAsString();
        QualType qt_param = param->getType();

        if (IsCallByLocation(qt_param)) {
            std::string sname = TranslateQualType(qt_param, TypeMode::var);

            outs() << "let Coq_step(" << pname << ", st) = Pervasives_impl.salloc_with_init_ " << sname << " " << pname << " st in\n";
        }

    }
    outs() << "!" << name_impl;

    if (CXXMethodDecl::classof(d)) {
        CXXMethodDecl *cxxmdecl = (CXXMethodDecl *) d;
        if (cxxmdecl->isInstance()) {
            QualType qt = cxxmdecl->getThisType(_cxt);
            outs() << " this";
        }
    }

    for (auto param : d-> parameters()) {
        outs() << " " << param->getNameAsString();
    }
    outs() << " st\".\n";

}

translateDecl::translateDecl(ASTContext &cxt) : Translate(cxt) {}

class TranslateDeclConsumer : public ASTConsumer {

public:

    explicit TranslateDeclConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateDecl(context).TranslateTranslationUnitDecl(context.getTranslationUnitDecl());
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

    outs() << "Require Import common.values.\n";
    outs() << "Require Import common.compt.\n";

    int result = tool.run(newFrontendActionFactory<TranslateDeclAction>().get());

    return result;
}
