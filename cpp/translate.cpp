#include "translate.hpp"

using namespace llvm;
using namespace clang;

std::string NameOfPath(const FunctionDecl *d) {
    return std::string { "" };
}

std::string NameOfFile(const FunctionDecl *d, const SourceManager *sm) {

    FunctionTemplateDecl *ftdecl = d->getPrimaryTemplate();
    if (ftdecl) {
        return std::string { "NameOfFile::ftdecl is not null" };
    }

    SourceLocation sloc = d->getOuterLocStart();

    std::string name = sm->getFilename(sloc);
    size_t pos = 0;
    while ((pos = name.find("/")) != std::string::npos) {
        name.erase(0, pos + 1);
    }

    pos = name.find(".");
    if (pos == std::string::npos) {
        return std::string { "NameOfFile::dot not found" };
    }
    name.erase(pos, name.length()-1);

    return name;
}

std::string StubFile(std::string s) {
    s[0] = toupper(s[0]);
    return std::string { s + "_stub" };
}

std::string ImplFile(std::string s) {
    s[0] = toupper(s[0]);
    return std::string { s + "_impl" };
}

std::string ConcatVector(std::vector<std::string> v, std::string s) {

    std::string ret = "";
    for (auto e : v) {
        ret += s + e;
    }
    return ret;
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

void Translate::TranslateFunctionTemplateDecl(FunctionTemplateDecl *d) {
        for (auto spec : d->specializations()) {
            TranslateFunctionDecl(spec);
        }
    }

void Translate::TranslateRecordDecl(RecordDecl *d) {
    outs() << "\n";

    std::string rname = NameOfRecordDecl(d);
    outs() << "Module " << rname << ".\n";

    for (auto decl : d->decls()) {
        TranslateDecl(decl);
    }

    outs() << "End " << rname << ".\n";
}


void Translate::TranslateDecl(const Decl *d) {
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

void Translate::TranslateDeclContext(const DeclContext *dc) {
    for (auto decl : dc->decls()) {
        TranslateDecl(decl);
    }
}


