#include "translate-impl.hpp"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory translateImplOptions("translate-impl options");

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

void ResetName() {
    count = 0;
}

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

std::string NameOfFieldDecl(FieldDecl *fdecl) {
    return fdecl->getNameAsString();
}

std::string PathOfFieldDecl(FieldDecl *fdecl) {
    return std::string { PathOfDeclContext(fdecl->getParent()) + NameOfFieldDecl(fdecl) };
}

std::string AccessPathOfValueDecl(ValueDecl *vdecl, const SourceManager &sm) {
    if (DeclaratorDecl::classof(vdecl)) {
        if (FieldDecl::classof(vdecl)) {
            FieldDecl *fdecl = (FieldDecl *) vdecl;
            return std::string
                { DeclFile(FileOfDecl(fdecl, sm)) + "." + PathOfFieldDecl(fdecl) };
        } else if (FunctionDecl::classof(vdecl)) {
            FunctionDecl *fdecl = (FunctionDecl *) vdecl;
            return std::string
                { DeclFile(FileOfDecl(fdecl, sm)) + "." + PathOfFunctionDecl(fdecl) };
        } else if (VarDecl::classof(vdecl)) {
            return vdecl->getNameAsString();
        }

        vdecl->dump();
        return std::string { "AccessPathOfValueDecl::DeclaratorDecl::else" };
    } else if (EnumConstantDecl::classof(vdecl)) {
        EnumConstantDecl *edecl = (EnumConstantDecl *) vdecl;
        return std::string
            { "int_" + (edecl->getInitVal()).toString(10) };
    } else {
        vdecl->dump();
        return std::string { "AccessPathOfValueDecl::else" };
    }
}

std::string NameOfBinaryOperator(BinaryOperator::Opcode op) {
    switch (op) {
        case BO_Mul:
            return std::string { "mul" };
        case BO_Div:
            return std::string { "div" };
        case BO_Rem:
            return std::string { "rem" };
        case BO_Add:
            return std::string { "add" };
        case BO_Sub:
            return std::string { "sub" };
        case BO_Shl:
            return std::string { "shl" };
        case BO_Shr:
            return std::string { "shr" };
        case BO_LT:
            return std::string { "lt" };
        case BO_GT:
            return std::string { "gt" };
        case BO_LE:
            return std::string { "le" };
        case BO_GE:
            return std::string { "ge" };
        case BO_EQ:
            return std::string { "eq" };
        case BO_NE:
            return std::string { "ne" };
        case BO_And:
            return std::string { "and" };
        case BO_Xor:
            return std::string { "xor" };
        case BO_Or:
            return std::string { "or" };
        case BO_LAnd:
            return std::string { "land" };
        case BO_LOr:
            return std::string { "lor" };
        case BO_Assign:
            return std::string { "assign" };
    }

    return std::string { "NameOfBinaryOperator::else" };
}

std::string NameOfUnaryOperator(UnaryOperator::Opcode op) {
    switch (op) {
    case UO_PostInc:
        return std::string { "postinc" };
    case UO_PostDec:
        return std::string { "postdec" };
    case UO_PreInc:
        return std::string { "preinc" };
    case UO_PreDec:
        return std::string { "predec" };
    case UO_AddrOf:
        return std::string { "addrof" };
    case UO_Deref:
        return std::string { "deref" };
    case UO_Plus:
        return std::string { "plus" };
    case UO_Minus:
        return std::string { "minus" };
    case UO_Not:
        return std::string { "not" };
    case UO_LNot:
        return std::string { "lnot" };
    }

    return std::string { "NameOfUnaryOperator::else" };
}


std::string translateImpl::TranslateExpr(Expr *e, std::string name) {
    QualType qt = e->getType();
    if (AbstractConditionalOperator::classof(e)) {
        if (ConditionalOperator::classof(e)) {
            ConditionalOperator *cexpr = (ConditionalOperator *) e;

            std::string name_ret = GenName(qt, name);
            outs() << name_ret << " <- ife _ _\n";

            outs() << "(";
            std::string cname = TranslateExpr(cexpr->getCond());
            outs() << "step _ "<< cname << ")\n";

            outs() << "(";
            std::string tname = TranslateExpr(cexpr->getTrueExpr());
            outs() << "step _ " << tname << ")\n";

            outs() << "(";
            std::string fname = TranslateExpr(cexpr->getFalseExpr());
            outs() << "step _ " << fname << ");\n";

            return name_ret;
        } else {
            e->dump();
            outs() << "TranslateExpr::AbstractConditionalOperator\n";
        }
    } else if (ArraySubscriptExpr::classof(e)) {
        ArraySubscriptExpr * aexpr = (ArraySubscriptExpr *) e;
        std::string name_base = TranslateExpr(aexpr->getBase());
        std::string name_idx = TranslateExpr(aexpr->getIdx());

        std::string name_ret = GenName(qt, name);
        outs() << name_ret << " <- array_subscript _ _ _ " << name_base << " " << name_idx << ";\n";
        return name_ret;
    } else if (BinaryOperator::classof(e)) {
        BinaryOperator *bexpr = (BinaryOperator *) e;
        Expr *lexpr = bexpr->getLHS();
        Expr *rexpr = bexpr->getRHS();

        std::string lname = TranslateExpr(lexpr);
        std::string lname_qt = TranslateQualType(lexpr->getType(), TypeMode::str);

        std::string rname = TranslateExpr(rexpr);
        std::string rname_qt = TranslateQualType(rexpr->getType(), TypeMode::str);

        BinaryOperator::Opcode op = bexpr->getOpcode();
        std::string oname = NameOfBinaryOperator(op);

        std::string name_ret = GenName(qt, name);
        outs() << name_ret << " <- " << oname << "_" << lname_qt << "_" << rname_qt << " _ " << lname << " " << rname << ";\n";
        return name_ret;
    } else if (CallExpr::classof(e)) {
        CallExpr *cexpr = (CallExpr *) e;
        std::string name_ret = GenName(qt, name);

        Expr* callee = cexpr->getCallee();
        std::string name_callee = TranslateExpr(callee);

        std::vector<std::string> anames;
        for (auto arg : cexpr->arguments()) {
            std::string aname = TranslateExpr(arg);
            anames.emplace_back(aname);
        }

        outs() << name_ret << " <- call _ _ (" << name_callee << ConcatVector2(anames, " ") << ");\n";

        return name_ret;

    } else if (CastExpr::classof(e)) {
        if (ExplicitCastExpr::classof(e)) {
            return std::string { "TranslateExpr::ExplicitCastExpr" };
        } else if (ImplicitCastExpr::classof(e)) {
            ImplicitCastExpr *expr_cast = (ImplicitCastExpr *) e;

            Expr *expr_sub = expr_cast->getSubExpr();
            std::string name_sub = TranslateExpr(expr_sub);

            CastKind kind = expr_cast->getCastKind();
            switch (kind) {
            case CK_ArrayToPointerDecay:
                {
                    std::string name_new
                    { "(array_to_pointer_decay _ " + name_sub + ")" };
                    return BindOrReturn(name_new, name);
                }
            case CK_FunctionToPointerDecay:
                {
                    return name_sub;
                }
            case CK_IntegralCast:
                {
                    std::string name_new = GenName(qt, name);
                    outs() << name_new << " <- integral_cast _ " << TranslateQualType(qt, TypeMode::var) << " _ " << name_sub << ";\n";
                    return name_new;
                }
            case CK_IntegralToBoolean:
                {
                    std::string name_new
                    { "(integral_to_boolean _ " + name_sub + ")" };
                    return BindOrReturn(name_new, name);
                }
            case CK_LValueToRValue:
                {
                    std::string name_new = GenName(qt, name);
                    outs() << name_new << " <- get _ _ " << name_sub << ";\n";
                    return name_new;
                }
            case CK_NoOp:
                {
                    return BindOrReturn(name_sub, name);
                }
            case CK_NullToPointer:
                {
                    std::string name_new
                    { "(null_to_pointer _ _ " + name_sub + ")" };
                    return BindOrReturn(name_new, name);
                }
            case CK_PointerToBoolean:
                {
                    std::string name_new
                    { "(pointer_to_boolean _ " + name_sub + ")" };
                    return BindOrReturn(name_new, name);
                }
            default:
                e->dump();
                return std::string { "ImplicitCastExpr::else" };
            }
        } else {
            return std::string { "TranslateExpr::ExplicitCastExpr" };
        }
    } else if (CXXConstructExpr::classof(e)) {
        CXXConstructExpr *cexpr = (CXXConstructExpr *) e;
        CXXConstructorDecl *cxxcdecl = cexpr->getConstructor();
        CXXRecordDecl *cxxrdecl = cxxcdecl->getParent();

        std::string fname = AccessPathOfValueDecl(cxxcdecl, _cxt.getSourceManager());

        std::vector<std::string> anames;

        std::string name_new = GenName(qt, name);
        std::string name_ty = NameOfRecordDecl(cxxrdecl);
        outs() << name_new << " <- salloc " << name_ty << " _;\n";
        anames.emplace_back(name_new);

        for (arg : cexpr->arguments()) {
            anames.emplace_back(TranslateExpr(arg));
        }

        outs() << "_ <- call _ _ (" << fname << ConcatVector2(anames, " ") << ");\n";
        return name_new;
    } else if (CXXNewExpr::classof(e)) {
        CXXNewExpr *nexpr = (CXXNewExpr *) e;
        std::string name_new = GenName(qt, name);

        for (auto args: nexpr->placement_arguments()) {
            outs() << "CXXNewExpr::placement_arguments\n";
        }

        QualType qt_alloc = nexpr->getAllocatedType();
        outs() << name_new << " <- halloc " << TranslateQualType(qt_alloc, TypeMode::var) << " _;\n";
        Expr *iexpr = nexpr->getInitializer();

        if (CXXConstructExpr::classof(iexpr)) {
            CXXConstructExpr *cexpr = (CXXConstructExpr *) iexpr;
            CXXConstructorDecl *cxxcdecl = cexpr->getConstructor();
            std::string fname = AccessPathOfValueDecl(cxxcdecl, _cxt.getSourceManager());
            std::vector<std::string> anames;
            for (arg : cexpr->arguments()) {
                anames.emplace_back(TranslateExpr(arg));
            }

            outs() << "_ <- call _ _ (" << fname << " " << name_new << ConcatVector2(anames, " ") << ");\n";
            return name_new;
        } else {
            return std::string { "TranslateExpr::CXXNewExpr::else" };
        }

    } else if (CXXThisExpr::classof(e)) {
        return std::string { "this" };
    } else if (DeclRefExpr::classof(e)) {
        DeclRefExpr *dexpr = (DeclRefExpr *) e;
        ValueDecl *vdecl = dexpr->getDecl();
        if (vdecl) {
            return AccessPathOfValueDecl(vdecl, _cxt.getSourceManager());
        } else {
            return std::string { "TranslateExpr::DeclRefExpr::null" };
        }
    } else if (IntegerLiteral::classof(e)) {
        IntegerLiteral *iexpr = (IntegerLiteral *) e;
        APInt i = iexpr->getValue();
        return BindOrReturn("int_" + i.toString(10, true), name);
    } else if (MemberExpr::classof(e)) {
        MemberExpr *mexpr = (MemberExpr *) e;
        std::string bname = TranslateExpr(mexpr->getBase());
        ValueDecl *vdecl = mexpr->getMemberDecl();
        return std::string { "(" + AccessPathOfValueDecl(vdecl, _cxt.getSourceManager()) + " " + bname + ")" };
    } else if (ParenExpr::classof(e)) {
        ParenExpr *pexpr = (ParenExpr *) e;
        Expr* sexpr = pexpr->getSubExpr();
        return TranslateExpr(sexpr);
    } else if (UnaryOperator::classof(e)) {
        UnaryOperator *uexpr = (UnaryOperator *) e;

        UnaryOperator::Opcode op = uexpr->getOpcode();
        std::string opname = NameOfUnaryOperator(op);

        Expr *sexpr = uexpr->getSubExpr();
        std::string sname = TranslateExpr(sexpr);
        std::string name_sqt = TranslateQualType(sexpr->getType(), TypeMode::str);
        std::string name_ret = GenName(qt, name);
        outs() << name_ret << " <- " << opname << "_" << name_sqt << " _ " << sname << ";\n";
        return name_ret;
    }

    e->dump();
    return std::string { "TranslateExpr::else" };
}

bool IsAlias(QualType qt) {
    const Type* ty = qt.getTypePtrOrNull();
    if (ty == NULL) {
        outs() << "IsAlias::NULL\n";
        return false;
    }
    return IsAlias(ty);
}

bool IsAlias(const Type *ty) {
    if (ReferenceType::classof(ty)) {
        return true;
    } else if (RecordType::classof(ty)) {
        return true;
    } else if (SubstTemplateTypeParmType::classof(ty)) {
        return IsAlias(((const SubstTemplateTypeParmType *) ty)->getReplacementType());
    }

    return false;
}

void translateImpl::TranslateDeclStmt(Decl *d) {
    if (VarDecl::classof(d)) {
        VarDecl *vdecl = (VarDecl *) d;
        QualType qt = vdecl->getType();
        std::string name = vdecl->getNameAsString();

        if (vdecl->hasInit()) {
            Expr *e = vdecl->getInit();
            std::string name_init = TranslateExpr(e);
            if (IsAlias(qt)) {
                outs() << "let " << name << " := " << name_init << " in\n";
            } else {
                outs() << name << " <- salloc_with_init " << TranslateQualType(qt, TypeMode::var) << " _ " << name_init << ";\n";
            }
        } else {
            outs() << "TranslateDeclStmt::VarDecl::hasInit false\n";
        }
    } else {
        outs() << "TranslateDeclStmt::else\n";
    }
}

void translateImpl::TranslateStmt(Stmt *s, StmtMode mode) {
    if (CompoundStmt::classof(s)) {
        CompoundStmt *cstmt = (CompoundStmt *) s;

        if (cstmt->size() == 0) {
            outs() << Prompt(mode) << "step _ ttt" << Punct(mode);
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
    } else if (Expr::classof(s)) {
        std::string name = TranslateExpr((Expr *) s);
        outs() << Prompt(mode) << "step _ ttt" << Punct(mode);
    } else if (ForStmt::classof(s)) {
        ForStmt *s_for = (ForStmt *) s;
        Stmt *s_init = s_for->getInit();
        Expr *e_cond = s_for->getCond();
        Stmt *s_inc = s_for->getInc();
        Stmt *s_body = s_for->getBody();

        if (s_init) {
            TranslateStmt(s_init, StmtMode::semicolon);
        }

        outs() << Prompt(mode) << "do_for _ _ _\n";

        outs() << "(";
        if (e_cond) {
            std::string name = TranslateExpr(e_cond);
            outs() << "step _ " << name;
        } else {
            outs() << "step _ tt";
        }
        outs() << ")\n";

        outs() << "(";
        if (s_inc) {
            TranslateStmt(s_inc, StmtMode::none);
        } else {
            outs() << "step _ ttt";
        }
        outs() << ")\n";

        outs() << "(";
        if (s_body) {
            TranslateStmt(s_body, StmtMode::none);
        } else {
            outs() << "step _ ttt";
        }
        outs() << ")";
        outs() << Punct(mode);
    } else if (IfStmt::classof(s)) {
        IfStmt *s_if = (IfStmt *) s;
        Stmt *s_init = s_if->getInit();
        Expr *e_cond = s_if->getCond();
        Stmt *s_then = s_if->getThen();
        Stmt *s_else = s_if->getElse();

        if (s_init) {
            TranslateStmt(s_init, StmtMode::semicolon);
        }

        outs() << Prompt(mode) << "do_if _\n";

        outs() << "(";
        if (e_cond) {
            std::string name = TranslateExpr(e_cond);
            outs() << "step _ " << name;
        } else {
        }
        outs() << ")\n";

        outs() << "(";
        if (s_then) {
            TranslateStmt(s_then, StmtMode::none);
        } else {
            outs() << "step _ ttt";
        }
        outs() << ")\n";

        outs() << "(";
        if (s_else) {
            TranslateStmt(s_else, StmtMode::none);
        } else  {
            outs() << "step _ ttt";
        }
        outs() << ")" << Punct(mode);

    } else if (ReturnStmt::classof(s)) {
        ReturnStmt *rstmt = (ReturnStmt *) s;

        Expr *e = rstmt->getRetValue();
        if (e) {
            std::string ename = TranslateExpr(e);
            outs() << Prompt(mode) << "ret _ " << ename << Punct(mode);
        } else {
            outs() << Prompt(mode) << "ret _ ttt" << Punct(mode);
        }
    } else if (WhileStmt::classof(s)) {
        WhileStmt *s_while = (WhileStmt *) s;
        Expr *e_cond = s_while->getCond();
        Stmt *s_body = s_while->getBody();

        outs() << Prompt(mode) << "do_while _ _\n";

        outs() << "(";
        if (e_cond) {
            std::string name = TranslateExpr(e_cond);
            outs() << "step _ " << name;
        } else {
            outs() << "step _ ttt";
        }
        outs() << ")\n";

        outs() << "(";
        if (s_body) {
            TranslateStmt(s_body, StmtMode::none);
        } else {
            outs() << "step _ ttt";
        }
        outs() << ")" << Punct(mode);
    } else {
        outs() << "TranslateStmt::else\n";
        s->dump();
    }
}

void translateImpl::TranslateFunctionDecl(const FunctionDecl *d) {
    ResetName();
    // if (d->getNameAsString() != "_child_depth" && d->getNameAsString() != "_bias") return;

    outs() << "\n";

    std::string fname = NameOfFunctionDecl(d);

    if (!d->hasBody()) {
        outs() << "(* " << fname << " has no body. *)\n";
        return;
    }

    outs() << "Definition " << fname;;

    if (CXXMethodDecl::classof(d)) {
        CXXMethodDecl *cxxmdecl = (CXXMethodDecl *) d;
        if (cxxmdecl->isInstance()) {
            QualType qt = cxxmdecl->getThisType(_cxt);
            outs() << " (this : value " << TranslateQualType(qt, TypeMode::var) << ")";
        }
    }

    for (auto param : d->parameters()) {
        std::string pname = param->getNameAsString();

        QualType qt_param = param->getType();
        std::string sname = TranslateQualType(qt_param, TypeMode::param);

        outs() << " (" << pname << " : value " << sname << ")";

    }

    QualType qt_ret = d->getReturnType();
    std::string str_ret = TranslateQualType(qt_ret, TypeMode::var);
    outs() << " : result " << str_ret << " :=\n";

    if (CXXConstructorDecl::classof(d)) {
        CXXConstructorDecl *cxxcdecl = (CXXConstructorDecl *) d;
        for (auto init : cxxcdecl->inits()) {
            if (init->isMemberInitializer()) {
                FieldDecl *fdecl = init->getMember();
                Expr *iexpr = init->getInit();
                std::string iname = TranslateExpr(iexpr);
                outs() << "_ <- put _ _ (" + AccessPathOfValueDecl(fdecl, _cxt.getSourceManager()) + " this) " <<iname << ";\n";
            } else {
            outs() << "TranslateFunctionDecl::CXXConstructorDecl\n";
            }
        }
    }

    Stmt* body = d->getBody();
    TranslateStmt(body, StmtMode::period);

}

void translateImpl::TranslateVarDecl(const VarDecl *d) {
    std::string vname = d->getNameAsString();
    outs() << "Definition " << vname << " :=\n";
}

translateImpl::translateImpl(ASTContext &cxt) : Translate(cxt) {}

class translateImplConsumer : public ASTConsumer {

public:

    explicit translateImplConsumer(CompilerInstance &_compiler, StringRef file)
        : fname(file), compiler(_compiler)
    {
    }

    virtual void HandleTranslationUnit(ASTContext &context) {
        translateImpl(context).TranslateTranslationUnitDecl(context.getTranslationUnitDecl());
    }

private:
    StringRef fname;
    CompilerInstance &compiler;

};

class translateImplAction : public ASTFrontendAction {
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef file) {
        return std::unique_ptr<ASTConsumer>(new translateImplConsumer(Compiler, file));
    }

};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, translateImplOptions);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    outs() << "Require Import mathcomp.ssreflect.ssreflect.\n";
    outs() << "Require Import common.values.\n";
    outs() << "Require Import common.compt.\n";
    outs() << "Require Import common.pervasives_decl.\n";

    int result = tool.run(newFrontendActionFactory<translateImplAction>().get());

    return result;
}
