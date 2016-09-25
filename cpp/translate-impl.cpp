#include "translate-impl.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory TranslateImplOptions("translate-impl options");

std::string ConcatVector(std::vector<std::string> v, std::string s = "") {

    std::string ret = "";
    for (auto e : v) {
        ret += s + e;
    }
    return ret;
}

std::string NameOfFunctionDecl(const FunctionDecl *d) {
    std::string fname = d->getNameAsString();

    std::vector<std::string> anames;
    for (auto param : d->parameters()) {
        anames.emplace_back(TranslateQualType(param->getType(), TypeMode::str));
    }

    return std::string { fname + ConcatVector(anames, "_") };
}

std::string NameOfRecordDecl(const RecordDecl *d) {
    return d->getNameAsString();
}

std::string TranslateBuiltinType(const BuiltinType *ty) {
    BuiltinType::Kind kind = ty->getKind();
    switch (kind) {
    case BuiltinType::Bool:
        return std::string { "Bool" };
    case BuiltinType::UChar:
        return std::string { "Uchar" };
    case BuiltinType::UInt:
        return std::string { "Uint" };
    case BuiltinType::ULong:
        return std::string { "Ulong" };
    case BuiltinType::ULongLong:
        return std::string { "Ulonglong" };
    case BuiltinType::Char_S:
        return std::string { "Char_s" };
    case BuiltinType::Int:
        return std::string { "Int" };
    case BuiltinType::Long:
        return std::string { "Long" };
    case BuiltinType::LongLong:
        return std::string { "Longlong" };
    case BuiltinType::NullPtr:
        return std::string { "Nullptr" };
    default:
        return std::string { "TranslateBuiltinType::default" };
    }
}

std::string TranslateType(const Type *ty, TypeMode mode) {
    if (BuiltinType::classof(ty)) {
        return TranslateBuiltinType((const BuiltinType *) ty);
    } else if (PointerType::classof(ty)) {
        std::string name_pointee = TranslateQualType(ty->getPointeeType(), mode);

        switch (mode) {
        case TypeMode::var:
            return std::string { "(Loc " + name_pointee + ")" };
        case TypeMode::str:
            return std::string { "Loc" + name_pointee };
        }
    } else if (SubstTemplateTypeParmType::classof(ty)) {
        const SubstTemplateTypeParmType * pty = (const SubstTemplateTypeParmType *) ty;
        return TranslateQualType(pty->getReplacementType(), mode);
    } else {
        ty->dump();
        return std::string { "TranslateType::else" };
    }
}

std::string TranslateQualType(const QualType qt, TypeMode mode) {
    const Type *ty = qt.getTypePtrOrNull();
    if (ty != NULL) {
        return TranslateType(ty, mode);
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

int count = 0;

std::string GenName(const Type *ty) {
    return std::string { "tmp_" + std::to_string(count++) };
}

std::string GenName(QualType qt) {
    const Type *ty = qt.getTypePtrOrNull();
    if (ty != NULL) {
        return GenName(ty);
    } else {
        return std::string { "GenName:NULL" };
    }
}

std::string GenName(QualType qt, std::string bname) {
    if (bname == "") {
        return GenName(qt);
    } else {
        return bname;
    }
}

std::string BindOrReturn(std::string name, std::string bname) {
    if (bname == "") {
        return name;
    } else {
        outs() << "let " << bname << " := " << name << " in\n";
        return bname;
    }
}

std::string StringOfValueDecl(ValueDecl *vdecl) {
    if (DeclaratorDecl::classof(vdecl)) {
        if (VarDecl::classof(vdecl)) {
            return vdecl->getNameAsString();
        } else {
            vdecl->dump();
            return std::string { "StringOfValueDecl::DeclaratorDecl::else" };
        }
    } else {
        vdecl->dump();
        return std::string { "StringOfValueDecl::else" };
    }

}

std::string TranslateExpr(Expr *e, std::string name = "") {
    QualType qt = e->getType();
    if (BinaryOperator::classof(e)) {
        BinaryOperator *bexpr = (BinaryOperator *) e;
        Expr *lexpr = bexpr->getLHS();
        Expr *rexpr = bexpr->getRHS();

        std::string lname = TranslateExpr(lexpr);
        std::string lname_qt = TranslateQualType(lexpr->getType(), TypeMode::str);

        std::string rname = TranslateExpr(rexpr);
        std::string rname_qt = TranslateQualType(rexpr->getType(), TypeMode::str);

        BinaryOperator::Opcode kind = bexpr->getOpcode();
        switch (kind) {
        case BO_Add:
            {
                std::string name_ret = GenName(qt, name);
                outs() << name_ret << " <- add_" << lname_qt << "_" << rname_qt << " _ " << lname << " " << rname << ";\n";
                return name_ret;
            }
        default:
            return std::string { "TranslateExpr::BinaryOperator::default" };
        }

    } else if (CastExpr::classof(e)) {
        if (ExplicitCastExpr::classof(e)) {
            return std::string { "TranslateExpr::ExplicitCastExpr" };
        } else if (ImplicitCastExpr::classof(e)) {
            ImplicitCastExpr *expr_cast = (ImplicitCastExpr *) e;

            Expr *expr_sub = expr_cast->getSubExpr();
            std::string name_sub = TranslateExpr(expr_sub);

            CastKind kind = expr_cast->getCastKind();
            switch (kind) {
            case CK_LValueToRValue:
                {
                    std::string name_new = GenName(qt, name);
                    outs() << name_new << " <- get _ _ " << name_sub << ";\n";
                    return name_new;
                }
            default:
                return std::string { "ImplicitCastExpr::else" };
            }
        } else {
            return std::string { "TranslateExpr::ExplicitCastExpr" };
        }
    } else if (DeclRefExpr::classof(e)) {
        DeclRefExpr *dexpr = (DeclRefExpr *) e;
        ValueDecl *vdecl = dexpr->getDecl();
        if (vdecl) {
            return StringOfValueDecl(vdecl);
        } else {
            return std::string { "TranslateExpr::DeclRefExpr::null" };
        }
    } else if (IntegerLiteral::classof(e)) {
        IntegerLiteral *iexpr = (IntegerLiteral *) e;
        APInt i = iexpr->getValue();
        return BindOrReturn("int_" + i.toString(10, true), name);
    }

    e->dump();
    return std::string { "TranslateExpr::else" };
}

void TranslateDeclStmt(Decl *d) {
    if (VarDecl::classof(d)) {
        VarDecl *vdecl = (VarDecl *) d;
    } else {
        outs() << "TranslateDeclStmt::else\n";
    }
}

void TranslateStmt(Stmt *s, StmtMode mode = StmtMode::semicolon) {
    if (CompoundStmt::classof(s)) {
        CompoundStmt *cstmt = (CompoundStmt *) s;

        if (cstmt->size() == 0) {
            outs() << "TranslateStmt::CompoundStmt:0\n";
            return;
        }

        for(CompoundStmt::body_iterator b = cstmt->body_begin(), e = cstmt->body_end();
            b != e; b++) {
            if (b+1 == e) {
                TranslateStmt(*b, mode);
            } else {
                TranslateStmt(*b);
            }
        }

    } else if (DeclStmt::classof(s)) {
        DeclStmt *dstmt = (DeclStmt *) s;

        for (auto decl : dstmt->decls()) {
            TranslateDeclStmt(decl);
        }

    } else if (ReturnStmt::classof(s)) {
        ReturnStmt *rstmt = (ReturnStmt *) s;

        Expr *e = rstmt->getRetValue();
        if (e) {
            std::string ename = TranslateExpr(e);
            outs() << Prompt(mode) << "return " << ename << Punct(mode);
        } else {
            outs() << Prompt(mode) << "return" << Punct(mode);
        }

    } else {
        outs() << "TranslateStmt::else\n";
        s->dump();
    }
}

void TranslateFunctionDecl(const FunctionDecl *d) {
    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    if (!d->hasBody()) {
        outs() << "(* " << fname << " has no body. *)\n";
    }

    outs() << "Definition " << fname;;

    if (d->param_empty()) {
        outs() << " (_ : value void)";
    } else {
        for (auto param : d->parameters()) {
            std::string pname = param->getNameAsString();

            QualType qt_param = param->getType();
            std::string sname = TranslateQualType(qt_param);

            outs() << " (" << pname << " : value " << sname << ")";

        }
    }

    QualType qt_ret = d->getReturnType();
    std::string str_ret = TranslateQualType(qt_ret);
    outs() << " : result " << str_ret << " :=\n";

    Stmt* body = d->getBody();
    TranslateStmt(body, StmtMode::period);

}

void TranslateFunctionTemplateDecl(FunctionTemplateDecl *d) {
    for (auto spec : d->specializations()) {
        TranslateFunctionDecl(spec);
    }
}

void TranslateRecordDecl(RecordDecl *d) {
    outs() << "\n";

    std::string rname = NameOfRecordDecl(d);
    outs() << "Module " << rname << ".\n";

    outs() << "End " << rname << ".\n";
}

void TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "Definition " << vname << " :=\n";
}

void TranslateDecl(const Decl *d) {
    if (TemplateDecl::classof(d)) {
        if (RedeclarableTemplateDecl::classof(d)) {
            if (FunctionTemplateDecl::classof(d)) {
                TranslateFunctionTemplateDecl((FunctionTemplateDecl *) d);
            } else {
                outs() << "TranslateDecl::RedecralableTemplateDecl::else\n";
            }
        } else {
            outs() << "TranslateDecl::TemplateDecl::else\n";
        }
    } else if (TypeDecl::classof(d)) {
        if (TagDecl::classof(d)) {
            if (RecordDecl::classof(d)) {
                TranslateRecordDecl((RecordDecl *) d);
            } else {
                outs() << "TranslateDecl::TagDecl::else\n";
            }
        } else {
            outs() << "TranslateDecl::TypeDecl::else\n";
        }
    } else if (NamedDecl::classof(d)) {
        if (ValueDecl::classof(d)) {
            if (DeclaratorDecl::classof(d)) {
                if (FunctionDecl::classof(d)) {
                    TranslateFunctionDecl((const FunctionDecl *) d);
                } else if (VarDecl::classof(d)) {
                    TranslateVarDecl((const VarDecl *) d);
                } else {
                    d->dump();
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
