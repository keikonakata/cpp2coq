#include "translate.hpp"

using namespace std;
using namespace llvm;
using namespace clang;

std::string ConcatVector(std::vector<std::string> v, std::string s) {

    std::size_t size = v.size();

    if (size == 0) {
        return std::string { "" };
    } else {
        std::string ret = v[0];
        for (auto i = 1; i < size; i++) {
            ret += s + v[i];
        }
        return ret;
    }
}

std::string ConcatVector2(std::vector<std::string> v, std::string s) {
    std::string ret = "";

    for (auto elm : v) {
        ret += s + elm;
    };

    return ret;
}

std::string ConcatVector3(std::vector<std::string> v, std::string s) {
    std::string ret = "";

    for (auto elm : v) {
        ret += elm + s;
    };

    return ret;
}

void PathOfDeclContextRec(const DeclContext *dcxt, std::vector<std::string> &names) {
    if (!dcxt) {
        return;
    } else {
        PathOfDeclContextRec(dcxt->getParent(), names);
    }

    if (dcxt->isNamespace()) {
        PathOfDeclContextRec(dcxt->getParent(), names);
    } else if (dcxt->isRecord()) {
        names.emplace_back(NameOfRecordDecl((RecordDecl *) TagDecl::castFromDeclContext(dcxt)));
    } else if (dcxt->isTranslationUnit()) {
        PathOfDeclContextRec(dcxt->getParent(), names);
    } else {
        names.emplace_back("PathOfDeclContextRec::else");
    }

}

// dcxt may be a nullptr
std::string PathOfDeclContext(const DeclContext *dcxt, std::string sep) {
    std::vector<std::string> names;
    PathOfDeclContextRec(dcxt, names);
    return ConcatVector3(names, sep);
}

std::string PathOfFunctionDecl(const FunctionDecl *fdecl) {
    const DeclContext *dcxt = fdecl->getParent();
    if (dcxt) {
        return std::string { PathOfDeclContext(dcxt) + NameOfFunctionDecl(fdecl) };
    } else {
        return NameOfFunctionDecl(fdecl);
    }
}

string notdir(string name) {
    size_t pos = 0;

    while ((pos = name.find("/")) != std::string::npos) {
        name.erase(0, pos + 1);
    }

    return name;
}

string basename(string name) {
    size_t pos = name.find(".");
    if (pos == std::string::npos) {
        return std::string { "FileOfDecl::dot not found" };
    }
    name.erase(pos, name.length()-1);

    return name;
}

std::string FileOfDecl(const FieldDecl *d, const SourceManager &sm) {
    SourceLocation sloc = d->getOuterLocStart();
    std::string name = sm.getFilename(sloc);

    return basename(notdir(name));
}

std::string FileOfDecl(const FunctionDecl *d, const SourceManager &sm) {

    FunctionTemplateDecl *ftdecl = d->getPrimaryTemplate();
    if (ftdecl) {
        SourceLocation sloc = (ftdecl->getSourceRange()).getBegin();
        string name = sm.getFilename(sloc);
        return basename(notdir(name));
    }

    SourceLocation sloc = d->getOuterLocStart();
    std::string name = sm.getFilename(sloc);

    return basename(notdir(name));

}

std::string DeclFile(std::string s) {
    return std::string { s + "_decl" };
}

std::string StubFile(std::string s) {
    s[0] = toupper(s[0]);
    return std::string { s + "_stub" };
}

std::string ImplFile(std::string s) {
    s[0] = toupper(s[0]);
    return std::string { s + "_impl" };
}

std::string TranslateBuiltinType(const BuiltinType *ty) {
    BuiltinType::Kind kind = ty->getKind();
    switch (kind) {
    case BuiltinType::Void:
        return std::string { "Void" };
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


string TypeOfRecordDecl(const RecordDecl *rdecl) {
    const DeclContext *cxt = rdecl->getParent();
    return std::string
        { PathOfDeclContext(cxt, "") + NameOfRecordDecl(rdecl) };
}

std::string TranslateType(const Type *ty, TypeMode mode) {
    if (ArrayType::classof(ty)) {
        const ArrayType *aty = (const ArrayType *) ty;
        QualType eqt = aty->getElementType();
        switch (mode) {
        case TypeMode::param:
            return std::string
                { "(Loc (Array " + TranslateQualType(eqt, TypeMode::var) + "))" };
        case TypeMode::var:
            return std::string
                { "(Array " + TranslateQualType(eqt, TypeMode::var) + ")" };
        case TypeMode::str:
            return std::string
                { "Array" + TranslateQualType(eqt, TypeMode::str) };
        }
    } else if (BuiltinType::classof(ty)) {
        switch (mode) {
        case TypeMode::param:
            {
                std::string tname = TranslateBuiltinType((const BuiltinType *) ty);
                return std::string { "(Loc " + tname + ")" };
            }
        case TypeMode::var:
            return TranslateBuiltinType((const BuiltinType *) ty);
        case TypeMode::str:
            return TranslateBuiltinType((const BuiltinType *) ty);
        }
    } else if (PointerType::classof(ty)) {
        switch (mode) {
        case TypeMode::param:
            {
                std::string pname = TranslateQualType(ty->getPointeeType(), TypeMode::var);
                return std::string { "(Loc (Loc " + pname + "))" };
            }
        case TypeMode::var:
            {
                std::string pname = TranslateQualType(ty->getPointeeType(), mode);
                return std::string { "(Loc " + pname + ")" };
            }
        case TypeMode::str:
            {
                std::string pname = TranslateQualType(ty->getPointeeType(), mode);
                return std::string { "Loc" + pname };
            }
        }
    } else if (ReferenceType::classof(ty)) {
        QualType pointee = ty->getPointeeType();
        if (LValueReferenceType::classof(ty)) {
            switch (mode) {
            case TypeMode::param:
            case TypeMode::var:
                return std::string { "(Loc " + TranslateQualType(pointee, TypeMode::var) + ")" };
            case TypeMode::str:
                return std::string { "LRef" + TranslateQualType(pointee, TypeMode::str) };
            }
        } else if (RValueReferenceType::classof(ty)) {
            switch (mode) {
            case TypeMode::param:
            case TypeMode::var:
                return std::string { "(Loc " + TranslateQualType(pointee, TypeMode::var) + ")" };
            case TypeMode::str:
                return std::string { "RRef" + TranslateQualType(pointee, TypeMode::str) };
            }
        } else {
            ty->dump();
            return std::string { "TranslateType::ReferenceType::else" };
        }
    } else if (SubstTemplateTypeParmType::classof(ty)) {
        const SubstTemplateTypeParmType * pty = (const SubstTemplateTypeParmType *) ty;
        return TranslateQualType(pty->getReplacementType(), mode);
    } else if (TagType::classof(ty)) {
            if (RecordType::classof(ty)) {
                RecordDecl *rdecl = ((const RecordType *) ty)->getDecl();
                if (rdecl) {
                    string rname = TypeOfRecordDecl(rdecl);
                    switch (mode) {
                    case TypeMode::var:
                        return rname;
                    case TypeMode::param:
                        return string { "(Loc " + rname + ")" };
                    case TypeMode::str:
                        return rname;
                    }
                }

                return std::string { "TranslateType::RecordType::null " };
            } else if (EnumType::classof(ty)) {
                return std::string { "TranslateType::EnumType" };
            } else {
                ty->dump();
                return std::string { "TranslateType::TagType::else" };
            }
    } else {
        ty->dump();
        return std::string { "TranslateType::else" };
    }
}

string TranslateQualType(const QualType qt, TypeMode mode) {
    QualType cqt = qt.getCanonicalType();
    const Type *ty = cqt.getTypePtrOrNull();
    if (ty != NULL) {
        return TranslateType(ty, mode);
    } else {
        return std::string { "NULL" };
    }
}


void TranslateTemplateArgument(const TemplateArgument &arg, vector<string> &names, TypeMode mode) {
    TemplateArgument::ArgKind kind = arg.getKind();
    switch (kind) {
    case TemplateArgument::Type:
        {
            QualType qt = arg.getAsType();
            names.emplace_back(TranslateQualType(qt, mode));
            break;
        }
    case TemplateArgument::Pack:
        {
            for (auto parg : arg.pack_elements()) {
                TranslateTemplateArgument(parg, names, mode);
            }
            break;
        }
    default:
            names.emplace_back("TranslateTemplateArgument::Else");
    }
}

void TranslateTemplateArgumentList(const TemplateArgumentList *args, vector<string> &names, TypeMode mode) {
    unsigned size = args->size();
    for (auto i = 0; i < size; i++) {
        TranslateTemplateArgument(args->get(i), names, mode);
    }
}

std::string NameOfFunctionDecl(const FunctionDecl *d) {

    std::string fname;
    if (CXXConstructorDecl::classof(d)) {
        fname = "ctor";
    } else if (CXXConversionDecl::classof(d)) {
        fname = "TO FIX::cxxconversion";
    } else if (CXXDestructorDecl::classof(d)) {
        fname = "dtor";
    } else {
        fname = d->getNameAsString();
    }

    std::vector<std::string> anames;

    FunctionTemplateDecl *ftdecl = d->getPrimaryTemplate();
    if (ftdecl) {
        const TemplateArgumentList *targs = d->getTemplateSpecializationArgs();
        if (targs) {
            TranslateTemplateArgumentList(targs, anames, TypeMode::str);
        } else {
            outs() << "NameOfFunctionDecl::TemplateArgumentList is NULL.\n";
        }
    }

    for (auto param : d->parameters()) {
        anames.emplace_back(TranslateQualType(param->getType(), TypeMode::str));
    }

    return std::string { fname + ConcatVector2(anames, "_") };
}

std::string NameOfRecordDecl(const RecordDecl *d) {
    if (ClassTemplateSpecializationDecl::classof(d)) {
        return std::string { "NameOfRecordDecl::else" };
    }
    return d->getNameAsString();
}

void Translate::TranslateFunctionTemplateDecl(FunctionTemplateDecl *d) {
        for (auto spec : d->specializations()) {
            TranslateFunctionDecl(spec);
        }
    }

void Translate::TranslateEnumDecl(EnumDecl *d) {}

void Translate::TranslateRecordDecl(RecordDecl *d) {
    if (d->isInjectedClassName()) {
        return;
    }

    outs() << "\n";

    std::string rname = NameOfRecordDecl(d);
    outs() << "Module " << rname << ".\n";

    for (auto decl : d->decls()) {
        TranslateDecl(decl);
    }

    outs() << "\n";
    outs() << "End " << rname << ".\n";
}

void Translate::TranslateNamespaceDecl(const NamespaceDecl *d) {
    for (auto decl : d->decls()) {
        TranslateDecl(decl);
    }
}

void Translate::TranslateDecl(const Decl *d) {
    if (AccessSpecDecl::classof(d)) {
        return;
    } else if (NamedDecl::classof(d)) {
        if (NamespaceDecl::classof(d)) {
            const NamespaceDecl *nsdecl = (const NamespaceDecl *) d;
            TranslateNamespaceDecl(nsdecl);
        } else if (TemplateDecl::classof(d)) {
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

            // const TypeDecl *tdecl = (const TypeDecl *) d;
            // SourceLocation loc = tdecl->getLocStart();
            // if (_cxt.getSourceManager().getMainFileID() != _cxt.getSourceManager().getFileID(loc)) return;

            if (TagDecl::classof(d)) {
                if (RecordDecl::classof(d)) {
                    TranslateRecordDecl((RecordDecl *) d);
                } else if (EnumDecl::classof(d)) {
                    TranslateEnumDecl((EnumDecl *) d);
                } else {
                    d->dump();
                    outs() << "TranslateDecl::TagDecl::else\n";
                }
            } else if (TypedefNameDecl::classof(d)) {
                if (TypedefDecl::classof(d)) {
                    return;
                } else {
                    d->dump();
                    outs() << "TranslateDecl::TypedefNameDecl::else\n";
                }
            } else {
                d->dump();
                outs() << "TranslateDecl::TypeDecl::else\n";
            }
        } else if (UsingDecl::classof(d)) {
            return;
        } else if (UsingDirectiveDecl::classof(d)) {
            return;
        } else if (UsingShadowDecl::classof(d)) {
            return;
        } else if (ValueDecl::classof(d)) {
            if (DeclaratorDecl::classof(d)) {
                // const DeclaratorDecl *ddecl = (const DeclaratorDecl *) d;
                // SourceLocation loc = ddecl->getOuterLocStart();
                // if (_cxt.getSourceManager().getMainFileID() != _cxt.getSourceManager().getFileID(loc)) return;

                if (FieldDecl::classof(d)) {
                    TranslateFieldDecl((const FieldDecl *) d);
                } else if (FunctionDecl::classof(d)) {
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
            d->dump();
            outs() << "TranslateDecl::NamedDecl::else\n";
        }
    } else {
        d->dump();
        outs() << "TranslateDecl::else\n";
    }
}

void Translate::TranslateFieldDecl(const FieldDecl *d) {}

void Translate::TranslateFunctionDecl(const FunctionDecl *d) {}

void Translate::TranslateVarDecl(const VarDecl *d) {}

void Translate::TranslateTranslationUnitDecl(const TranslationUnitDecl *dc) {
    for (auto decl : dc->decls()) {
        TranslateDecl(decl);
    }
}


Translate::Translate(ASTContext &cxt) : _cxt(cxt) { }
