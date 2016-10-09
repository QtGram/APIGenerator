#include "typegenerator.h"
#include <QDebug>

TypeGenerator::TypeGenerator(const TelegramSchema &schema, const TelegramConstructors &constructors, const QString &destinationdir): BasicGenerator(schema, constructors, destinationdir)
{
    this->_manualitems["MessageContainer"] = "../../types/mtproto/messagecontainer.h";
    this->_manualitems["MTProtoMessage"] =  "../../types/mtproto/mtprotomessage.h";
    this->_manualitems["MTProtoObject"] =  "../../types/mtproto/mtprotoobject.h";
    this->_manualitems["MessageCopy"] =  "../../types/mtproto/messagecopy.h";
    this->_manualitems["RpcResult"] =  "../../types/mtproto/rpcresult.h";
    this->_manualitems["True"] = QString();
    this->_manualitems["TLBool"] = QString();
}

void TypeGenerator::generate()
{
    if(!this->_schema.contains("types"))
        return;

    this->iterateTypes(this->_schema["types"], [this](const SchemaItem* item) {
        if(this->_manualitems.contains(item->name()))
            return;

        ClassDeclarationStatement cdecl(item->name(), this->_destinationdir);
        LAMBDA_POINTER_D(ClassDeclarationStatement, cdecl);

        cdecl.setBaseClass("TelegramObject");

        cdecl.includes([](QSet<QString>& includelist) {
            includelist << "#include \"../../types/basic.h\"";
            includelist << "#include \"../../types/telegramobject.h\"";
        });

        cdecl.enums([this, item](QList<EnumDeclarationStatement*>& enumlist) {
            EnumDeclarationStatement* eds = new EnumDeclarationStatement("Constructors");

            foreach(SchemaItem* ctoritem, this->_constructors[item->name()])
                eds->addEntry("Ctor" + ctoritem->ctor(), "0x" + ctoritem->value());

            enumlist << eds;
        });

        cdecl.methods([this, LAMBDA_POINTER_V(cdecl)](QList<MethodDeclarationStatement*>& methodlist) {
            MethodDeclarationStatement* mds = new MethodDeclarationStatement(LAMBDA_POINTER_V(cdecl)->name());
            methodlist << mds;

            mds = new MethodDeclarationStatement("void", "read", MethodDeclarationStatement::VIRTUAL);
            mds->addArgument("MTProtoStream*", "mtstream");
            methodlist << mds;

            mds = new MethodDeclarationStatement("void", "write", MethodDeclarationStatement::VIRTUAL);
            mds->addArgument("MTProtoStream*", "mtstream");
            methodlist << mds;

            mds = new MethodDeclarationStatement("void", "compileFlags", MethodDeclarationStatement::VIRTUAL);
            mds->setVisibility(MethodDeclarationStatement::PROTECTED);
            methodlist << mds;
        });

        this->iterateFields(item, [this, LAMBDA_POINTER_V(cdecl)](const SchemaItem::SchemaField& field) {
            LAMBDA_POINTER_V(cdecl)->includes([this, field](QSet<QString>& includelist) {
                if(IS_FIELD_BASIC_TYPE(field))
                    return;

                if(this->_manualitems.contains(field.type)) {
                    if(!this->_manualitems[field.type].isEmpty())
                        includelist << this->_manualitems[field.type];

                    return;
                }

                if(IS_FIELD_VECTOR(field)) {
                    includelist << field.vectortype.toLower() + ".h";
                    return;
                }

                includelist << field.type.toLower() + ".h";
            });
        }, false, false);

        this->iterateFields(item, [this, LAMBDA_POINTER_V(cdecl)](const SchemaItem::SchemaField& field) {
            LAMBDA_POINTER_V(cdecl)->properties([this, field](QList<PropertyDeclarationStatement*>& proplist) {
                PropertyDeclarationStatement* prop = new PropertyDeclarationStatement(this->decoratedType(field), field.name);
                prop->setIsVector(IS_FIELD_VECTOR(field));
                proplist << prop;
            });
        }, false, true);

        ClassDefinitionStatement cdef(&cdecl, this->_destinationdir);
        LAMBDA_POINTER_D(ClassDefinitionStatement, cdef);

        cdef.constructor([this, item, LAMBDA_POINTER_V(cdef)](QString& body) {
            LAMBDA_POINTER_D(QString, body);

            this->iterateFields(item, [this, LAMBDA_POINTER_V(body)](const SchemaItem::SchemaField& field) {
                QString init = this->initValue(field);

                if(init.isEmpty())
                    return;

                LAMBDA_POINTER_V(body)->append("this->_" + field.name.toLower() + " = " + init + ";\n");
            }, false, true);


            const SchemaList& schemalist = this->_constructors[item->name()];

            if(schemalist.length() > 1)
                return;

            // Single constructors hardcode its id by default
            QString enumval = LAMBDA_POINTER_V(cdef)->name() + "::Ctor" + item->ctor();
            body += "this->_constructorid = " + enumval + ";\n";
        });

        cdef.method("read", [this, item, LAMBDA_POINTER_V(cdef)](QString& body) {
            body += "this->_constructorid = mtstream->readTLConstructor();\n\n";

            AssertStatement asrt("this->_constructorid");
            IfStatement ifs("this->_constructorid");

            foreach(SchemaItem* ctoritem, this->_constructors[item->name()]) {
                QString enumval = LAMBDA_POINTER_V(cdef)->name() + "::Ctor" + ctoritem->ctor();
                asrt.addAssertion(enumval);

                if(ctoritem->fields().isEmpty()) // Ignore parameter-less constructors
                    continue;

                ifs.addIf("==", enumval, [this, ctoritem](QString& body) {
                    this->compileReadConstructor(ctoritem, body);
                });
            }

            body += asrt.toString() + "\n\n";
            body += ifs.toString();
        });

        cdef.method("compileFlags", [this, item, LAMBDA_POINTER_V(cdef)](QString& body) {
            bool needsflagreset = true;
            IfStatement ifs("this->_constructorid");

            foreach(SchemaItem* ctoritem, this->_constructors[item->name()]) {
                if(!ctoritem->containsFlags())
                    continue;

                if(needsflagreset) {
                    body += "this->_flags = 0;\n\n";
                    needsflagreset = false;
                }

                QString enumval = LAMBDA_POINTER_V(cdef)->name() + "::Ctor" + ctoritem->ctor();

                ifs.addIf("==", enumval, [this, ctoritem](QString& body) {
                    this->compileFlags(ctoritem, body);
                });
            }

            body += ifs.toString();
        });

        cdef.method("write", [this, item, LAMBDA_POINTER_V(cdef)](QString& body) {
            AssertStatement asrt("this->_constructorid");
            IfStatement ifs("this->_constructorid");

            foreach(SchemaItem* ctoritem, this->_constructors[item->name()]) {
                QString enumval = LAMBDA_POINTER_V(cdef)->name() + "::Ctor" + ctoritem->ctor();
                asrt.addAssertion(enumval);

                if(ctoritem->fields().isEmpty()) // Ignore parameter-less constructors
                    continue;

                ifs.addIf("==", enumval, [this, ctoritem](QString& body) {
                    this->compileWriteConstructor(ctoritem, body);
                });
            }

            body += asrt.toString() + "\n\n";
            body += "this->compileFlags();\n";
            body += "mtstream->writeTLConstructor(this->_constructorid);\n\n";
            body += ifs.toString();
        });

        cdecl.write();
        cdef.write();
    });

    this->generateInclude();
}

void TypeGenerator::generateInclude()
{
    MasterIncludeStatement mstinclstmt(this->_destinationdir);
    LAMBDA_POINTER_D(MasterIncludeStatement, mstinclstmt);

    this->iterateTypes(this->_schema["types"], [this, LAMBDA_POINTER_V(mstinclstmt)](const SchemaItem* item) {
        if(this->_manualitems.contains(item->name()) && this->_manualitems[item->name()].isEmpty())
            return;

        LAMBDA_POINTER_V(mstinclstmt)->addInclude(item->name().toLower() + ".h");
    });

    foreach(const QString& manualitem, this->_manualitems.values())
    {
        if(manualitem.isEmpty())
            continue;

        mstinclstmt.addInclude(manualitem);
    }

    mstinclstmt.write();
}

void TypeGenerator::compileFlags(const SchemaItem *ctoritem, QString &ifbody)
{
    foreach(const SchemaItem::SchemaField& field, ctoritem->fields())
    {
        if(!IS_FIELD_FLAG(field))
            continue;

        IfStatement ifs;
        QString condition;

        if((field.type == "TLString") || (field.type == "TLBytes") || IS_FIELD_VECTOR(field))
            condition = "!this->_" + field.name.toLower() + ".isEmpty()";
        else
            condition = "this->_" + field.name.toLower();

        ifs.addIf(condition, [field](QString& body) {
            body += "SET_FLAG_BIT(this->_" + field.fieldref + ", " + QString::number(field.bitno) + ");";
        });

        ifbody += ifs.toString();
    }
}

void TypeGenerator::compileReadConstructor(const SchemaItem *ctoritem, QString &body)
{
    foreach(const SchemaItem::SchemaField& field, ctoritem->fields())
    {
        if(IS_FIELD_FLAG(field))
        {
            if(IS_FIELD_TRUE(field))
            {
                body += "this->_" + field.name.toLower() + " = IS_FLAG_SET(this->_" + field.fieldref + ", " + QString::number(field.bitno) + ");\n";
                continue;
            }

            IfStatement ifs;

            ifs.addIf("IS_FLAG_SET(this->_" + field.fieldref + ", " + QString::number(field.bitno) + ")", [this, field](QString& body) {
                this->readField(field, body);
            });

            body += ifs.toString() + "\n";
            continue;
        }

        this->readField(field, body);
    }
}

void TypeGenerator::compileWriteConstructor(const SchemaItem *ctoritem, QString &body)
{
    foreach(const SchemaItem::SchemaField& field, ctoritem->fields())
    {
        if(IS_FIELD_FLAG(field))
        {
            if(IS_FIELD_TRUE(field))
                continue;

            IfStatement ifs;

            ifs.addIf("IS_FLAG_SET(this->_" + field.fieldref + ", " + QString::number(field.bitno) + ")", [this, field](QString& body) {
                this->writeField(field, body);
            });

            body += ifs.toString() + "\n";
            continue;
        }

        this->writeField(field, body);
    }
}

void TypeGenerator::readField(const SchemaItem::SchemaField &field, QString &body)
{
    if(IS_FIELD_BASIC_TYPE(field) || IS_FIELD_VECTOR(field))
    {
        if(IS_FIELD_VECTOR(field))
            body += "mtstream->readTLVector<" + field.vectortype + ">(this->_" + field.name.toLower() + ", " + BARE_TYPE_STRING(field) + ");\n";
        else
            body += "this->_" + field.name.toLower() + " = mtstream->read" + field.type + "();\n";
    }
    else
    {
        QString varname = field.name.toLower() + "_ctor";
        body += "TLInt " + varname + " = mtstream->peekTLConstructor();\n\n";

        IfStatement ifs(varname);

        ifs.addIf("!=", "TLTypes::Null", [field](QString& body) {
            body += "RESET_TLTYPE(" + field.type + ", this->_" + field.name.toLower() + ");\n";
            body += "this->_" + field.name.toLower() + "->read(mtstream);\n";
        });

        ifs.setElse([field](QString& body) {
            body += "NULL_TLTYPE(this->_" + field.name.toLower() + ");\n";
            body += "mtstream->readTLConstructor(); // Skip Null";
        });

        body += ifs.toString() + "\n";
    }
}

void TypeGenerator::writeField(const SchemaItem::SchemaField &field, QString &body)
{
    if(IS_FIELD_BASIC_TYPE(field) || IS_FIELD_VECTOR(field))
    {
        if(IS_FIELD_VECTOR(field))
            body += "mtstream->writeTLVector(this->_" + field.name.toLower() + ", " + BARE_TYPE_STRING(field) + ");\n";
        else
            body += "mtstream->write" + field.type + "(this->_" + field.name.toLower() + ");\n";
    }
    else
    {
        IfStatement ifs("this->_" + field.name.toLower());

        ifs.addIf("!=", "NULL", [field](QString& body) {
            body += "this->_" + field.name.toLower() + "->write(mtstream);";
        });

        ifs.setElse([](QString& body) {
            body += "mtstream->writeTLConstructor(TLTypes::Null);";
        });

        body += ifs.toString() + "\n";
    }
}
