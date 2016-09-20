#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateImplOptions("translate-impl options");

std::string NameOfFunctionDecl(const FunctionDecl *d) {
    return d->getNameAsString();
}

std::string TranslateBuiltinType(const BuiltinType *ty) {
    BuiltinType::Kind kind = ty->getKind();
    switch (kind) {
    case BuiltinType::Int:
        return std::string { "int" };
    default:
        return std::string { "TranslateBuiltinType::default" };
    }
}

std::string TranslateType(const Type *ty) {
    if (BuiltinType::classof(ty)) {
        return TranslateBuiltinType((const BuiltinType *) ty);
    } else {
        return std::string { "TranslateType::else" };
    }
}

std::string TranslateQualType(const QualType qt) {
    const Type *ty = qt.getTypePtrOrNull();
    if (ty != NULL) {
        return TranslateType(ty);
    } else {
        return std::string { "NULL" };
    }
}

enum class StmtMode { semicolon, period, none };

std::string Prompt(StmtMode mode) {
    switch (mode) {
    case StmtMode::semicolon:
        return std::string { "_ <- " };
    case StmtMode::period:
    case StmtMode::none:
        return std::string { "" };
    }
}

std::string Punct(StmtMode mode) {
    switch (mode) {
    case StmtMode::semicolon:
        return std::string { ";\n" };
    case StmtMode::period:
        return std::string { ".\n" };
    case StmtMode::none:
        return std::string { "" };
    }
}

void TranslateStmt(Stmt *s, StmtMode mode) {
    if (CompoundStmt::classof(s)) {
        CompoundStmt *cstmt = (CompoundStmt *) s;
        
    } else if (ReturnStmt::classof(s)) {
        ReturnStmt *rstmt = (ReturnStmt *) s;
        Expr *e = rstmt->getRetValue();
        if (e) {
        } else {
        }
        outs() << Prompt(mode) << "return" << Punct(mode);
    } else {
        outs() << "TranslateStmt::else\n";
        s->dump();
    }
}

void TranslateFunctionDecl(const FunctionDecl *d) {
    std::string fname = NameOfFunctionDecl(d);

    if (!d->hasBody()) {
        outs() << "(* " << fname << " has no body. *)\n";
    }

    outs() << "Definition " << fname;;

    if (d->param_empty()) {
        outs() << " (_ : value void)";
    } else {
    }

    QualType qt_ret = d->getReturnType();
    std::string str_ret = TranslateQualType(qt_ret);
    outs() << " : value " << str_ret << " :=\n";

    Stmt* body = d->getBody();
    TranslateStmt(body, StmtMode::period);

}

void TranslateDecl(const Decl *d) {
    if (NamedDecl::classof(d)) {
        if (ValueDecl::classof(d)) {
            if (DeclaratorDecl::classof(d)) {
                if (FunctionDecl::classof(d)) {
                    TranslateFunctionDecl((const FunctionDecl *) d);
                } else {
                    outs() << "TranslateDecl::DeclaratorDecl::else\n";
                }
            } else {
                outs() << "TranslateDecl::ValueDecl::else\n";
            }
        } else {
            outs() << "TranslateDecl::NamedDecl::else\n";
        }
    } else {
        outs() << "TranslateDecl::else\n";
    }
}

void TranslateDeclContext(const DeclContext *dc) {
    for (auto decl : dc->decls()) {
        TranslateDecl(decl);
    }
}

class TranslateImplConsumer : public ASTConsumer {

public:

    explicit TranslateImplConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        TranslateDeclContext(context.getTranslationUnitDecl());
    }

private:
    StringRef fname;
    CompilerInstance &compiler;

};

class TranslateImplAction : public ASTFrontendAction {
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef file) {
        return std::unique_ptr<ASTConsumer>(new TranslateImplConsumer(Compiler, file));
                                            
    }

};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, TranslateImplOptions);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    int result = tool.run(newFrontendActionFactory<TranslateImplAction>().get());

    return result;
}
