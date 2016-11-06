#include "decompilergenerator.h"
#include "../schema/typeutils.h"

DecompilerGenerator::DecompilerGenerator(const TelegramSchema &schema, const TelegramConstructors &constructors, const QString &destinationdir): BasicGenerator(schema, constructors, destinationdir)
{
    this->_manualitems << "MessageContainer";
    this->_manualitems << "MTProtoObject";
    this->_manualitems << "MTProtoMessage";
    this->_manualitems << "MessageCopy";
    this->_manualitems << "RpcResult";
    this->_manualitems << "True";
    this->_manualitems << "TLBool";
}

void DecompilerGenerator::generate()
{
    this->generateTable();
    this->generateDecompiler();
}

void DecompilerGenerator::decompileField(const SchemaItem::SchemaField &field, const TelegramSchema& schema, bool *islast, QString& body)
{
    if(IS_FIELD_VECTOR(field))
    {
        body += "result.append(\"" + field.name.toLower() + ":\");\n";

        if(IS_FIELD_BASIC_TYPE(field))
            body += "thethis->decompileTLVector<" + field.vectortype + ">(result, mtstream, " + BARE_TYPE_STRING(field) + ");\n";
        else
            body += "thethis->decompileTLVectorEx(result, mtstream, " + BARE_TYPE_STRING(field) + ");\n";

        if(!(*islast))
            body += "result.append(\", \");\n";

        return;
    }

    if(IS_FIELD_BASIC_TYPE(field))
    {
        body += field.type + " " + field.name.toLower() + " = mtstream.read" + field.type + "();\n";

        if((field.type == "TLString") || (field.type == "TLBytes"))
            body += "result.append(\"" + field.name.toLower() + ":\" + thethis->printableString(" + field.name.toLower() + "));\n";
        else if((field.type == "TLInt128") || (field.type == "TLInt256"))
            body += "result.append(\"" + field.name.toLower() + ":\" + QString::fromUtf8(QByteArray::fromRawData(reinterpret_cast<const char*>(&" + field.name.toLower() + "), sizeof(" + field.type + ")).toHex()));\n";
        else if(field.type == "TLDouble")
            body += "result.append(\"" + field.name.toLower() + ":\" + QString::number(" + field.name.toLower() + "));\n";
        else
            body += "result.append(\"" + field.name.toLower() + ":\" + QString::number(" + field.name.toLower() + ", 16));\n";

        if(!(*islast))
            body += "result.append(\", \");\n";

        return;
    }

    SchemaItem* fielditem = this->fieldToItem(field, schema);
    Q_ASSERT(fielditem != NULL);

    body += "result.append(\"" + field.name.toLower() + ":\");\n";
    body += "MTProtoDecompiler::decompile_" + fielditem->name() + "(thethis, result, mtstream);\n";

}

void DecompilerGenerator::compileTable(QString& body, const TelegramSchema& schema)
{
    QList<QString> keys = schema.keys();

    foreach(const QString& key, keys)
    {
        foreach(SchemaItem* item, schema[key])
        {
            if(this->_manualitems.contains(item->ctor()))
                continue;

            if(key == "types")
                body += "DecompilerTable::_constructors[TLTypes::" + item->ctor() + "] = \"" + item->ctor() + "\";\n";
            else
                body += "DecompilerTable::_constructors[TLTypes::" + TypeUtils::apiCall(item->ctor()) + "] = \"" + item->ctor() + "\";\n";
        }
    }
}

void DecompilerGenerator::compileDispatchers(QString &body, const TelegramSchema &schema)
{
    QList<QString> keys = schema.keys();

    foreach(const QString& key, keys)
    {
        foreach(SchemaItem* item, schema[key])
        {
            if(key == "types")
            {
                if(this->_manualitems.contains(item->name()))
                    continue;

                body += "MTProtoDecompiler::_ctordispatcher[TLTypes::" + item->ctor() + "] = &MTProtoDecompiler::decompile_" + item->name() + ";\n";
            }
            else
                body += "MTProtoDecompiler::_ctordispatcher[TLTypes::" + TypeUtils::apiCall(item->ctor()) + "] = &MTProtoDecompiler::decompile_" + TypeUtils::apiCall(item->ctor()) + ";\n";
        }
    }
}

void DecompilerGenerator::compileDispatchersDeclarations(QList<MethodDeclarationStatement *> &methodlist, const TelegramSchema &schema)
{
    QList<QString> keys = schema.keys();

    foreach(const QString& key, keys)
    {
        if(key == "types")
        {
            LAMBDA_POINTER_D(QList<MethodDeclarationStatement*>, methodlist);

            this->iterateTypes(schema[key], [this, LAMBDA_POINTER_V(methodlist)](const SchemaItem* item) {
                if(this->_manualitems.contains(item->name()))
                    return;

                MethodDeclarationStatement* mds = new MethodDeclarationStatement("void", "decompile_" + item->name(), MethodDeclarationStatement::STATIC);
                mds->setVisibility(MethodDeclarationStatement::PRIVATE);
                mds->addArgument("MTProtoDecompiler*", "thethis");
                mds->addArgument("QString&", "result");
                mds->addArgument("MTProtoStream&", "mtstream");

                LAMBDA_POINTER_V(methodlist)->append(mds);
            });

            continue;
        }

        foreach(SchemaItem* item, schema[key])
        {
            MethodDeclarationStatement* mds = new MethodDeclarationStatement("void", "decompile_" + TypeUtils::apiCall(item->ctor()), MethodDeclarationStatement::STATIC);
            mds->setVisibility(MethodDeclarationStatement::PRIVATE);
            mds->addArgument("MTProtoDecompiler*", "thethis");
            mds->addArgument("QString&", "result");
            mds->addArgument("MTProtoStream&", "mtstream");
            methodlist << mds;
        }
    }
}

void DecompilerGenerator::compileDispatchersDefinitions(ClassDefinitionStatement *cdef, const TelegramSchema &schema, const TelegramConstructors& constructors)
{
    QList<QString> keys = schema.keys();

    foreach(const QString& key, keys)
    {
        if(key == "types")
        {
            this->iterateTypes(schema[key], [this, cdef, schema, constructors](const SchemaItem* item) {
                if(this->_manualitems.contains(item->name()))
                    return;

                cdef->method("decompile_" + item->name(), [this, item, schema, constructors](QString& body) {
                    this->typeDispatcher(body, item, schema, constructors);
                });
            });

            continue;
        }

        foreach(SchemaItem* item, schema[key])
        {
            cdef->method("decompile_" + TypeUtils::apiCall(item->ctor()), [this, item, key, schema](QString& body) {
                if(key == "functions") {
                    this->functionDispatcher(body, item, schema);
                    return;
                }

                body += "Q_UNUSED(thethis);\n";
                body += "Q_UNUSED(result);\n";
                body += "Q_UNUSED(mtstream);\n\n";
                body += "qDebug() << Q_FUNC_INFO << \"NOT IMPLEMENTED\";";
            });
        }
    }
}

void DecompilerGenerator::compileTLVectorDefinitions(QString &body)
{
    IfStatement ifs;

    ifs.addIf("!isbaretype", [](QString& body) {
        body += "ctor = mtstream.readTLConstructor();\n";
        body += "Q_ASSERT(ctor == TLTypes::Vector);\n\n";
    });

    body += "TLConstructor ctor = 0;\n\n";
    body += ifs.toString() + "\n\n";
    body += "TLInt length = mtstream.readTLInt();\n";
    body += "Q_ASSERT(length >= 0);\n";

    ForStatement forst("i", "length", "++");

    forst.setBody([this](QString& forbody) {
        IfStatement ifs("ctor");

        foreach(const SchemaItem* ctoritem, this->_schema["types"]) {
            if(this->_manualitems.contains(ctoritem->name()))
                continue;

            QString enumval = ctoritem->name() + "::Ctor" + ctoritem->ctor();

            ifs.addIf("==", enumval, [this, ctoritem](QString& ifbody) {
                ifbody += "MTProtoDecompiler::decompile_" + ctoritem->name() + "(this, result, mtstream);\n";
            });
        }

        ifs.setElse([](QString& body){
            body += "Q_ASSERT(false);";
        });

        forbody += "ctor = mtstream.peekTLConstructor();\n\n";
        forbody += ifs.toString();
    });

    body += forst.toString();
}

void DecompilerGenerator::typeDispatcher(QString &body, const SchemaItem *item, const TelegramSchema& schema, const TelegramConstructors& constructors)
{
    body += "\tQ_UNUSED(thethis);\n";
    body += "TLConstructor ctor = mtstream.readTLConstructor();\n\n";

    IfStatement ifs("ctor");

    ifs.addIf("==", "TLTypes::Null", [](QString& body) {
        body += "result.append(\"Null\");\n";
        body += "return;\n";
    });

    body += ifs.toString() +"\n\n";

    AssertStatement asrt("ctor");
    ifs = IfStatement("ctor");

    foreach(const SchemaItem* ctoritem, constructors[item->name()])
    {
        QString enumval = item->name() + "::Ctor" + ctoritem->ctor();
        asrt.addAssertion(enumval);

        ifs.addIf("==", enumval, [this, ctoritem, schema](QString& body) {
            if(ctoritem->fields().isEmpty())
                body += "result.append(\"" + ctoritem->ctor() + "\");\n";
            else
                body += "result.append(\"" + ctoritem->ctor() + "#\");\n";

            this->fieldDispatcher(body, ctoritem, schema);
        });
    }

    body += asrt.toString() + "\n\n";
    body += ifs.toString() + "\n";
}

void DecompilerGenerator::functionDispatcher(QString &body, const SchemaItem *item, const TelegramSchema& schema)
{
    body += "\tQ_UNUSED(thethis);\n";
    body += "TLConstructor ctor = mtstream.readTLConstructor();\n\n";

    AssertStatement asrt("ctor");
    asrt.addAssertion("TLTypes::" + item->ctor());

    body += asrt.toString() + "\n";

    if(!item->fields().isEmpty())
    {
        body += "result.append(\"" + item->ctor() + "(\");\n";
        this->fieldDispatcher(body, item, schema);
        body += "result.append(\")\");\n";
    }
    else
        body += "result.append(\"" + item->ctor() + "()\");\n";
}

void DecompilerGenerator::fieldDispatcher(QString &body, const SchemaItem *item, const TelegramSchema& schema)
{
    const SchemaItem::SchemaFields& fields = item->fields();

    for(int i = 0; i < fields.length(); i++)
    {
        const SchemaItem::SchemaField& field = fields[i];
        bool islast = (i == (fields.length() - 1));

        if(IS_FIELD_FLAG_KEEPER(field))
        {
            body += "\n";
            body += field.type + " " + field.name.toLower() + " = mtstream.read" + field.type + "();\n";
            body += "Q_UNUSED(" + field.name + ");\n\n";
            continue;
        }

        if(IS_FIELD_FLAG(field))
        {
            if(IS_FIELD_TRUE(field))
            {
                body += "result.append(\"" + field.name.toLower() + ":\" + BIT_FIELD_VALUE(" + field.fieldref + ", " + QString::number(field.bitno) + "));\n";

                if(!islast)
                    body += "result.append(\", \");\n";

                continue;
            }

            IfStatement ifs;
            LAMBDA_POINTER_D(bool, islast);

            ifs.addIf("IS_FLAG_SET(" + field.fieldref + ", " + QString::number(field.bitno) + ")", [this, field, schema, LAMBDA_POINTER_V(islast)](QString& body) {
                this->decompileField(field, schema, LAMBDA_POINTER_V(islast), body);
            });

            body += ifs.toString();
            continue;
        }

        this->decompileField(field, schema, &islast, body);
    }
}

void DecompilerGenerator::generateTable()
{
    ClassDeclarationStatement cdecl("DecompilerTable", this->_destinationdir);
    ClassDefinitionStatement cdef(&cdecl, this->_destinationdir);

    cdecl.includes([](QSet<QString>& includelist) {
        includelist << "<QObject>";
        includelist << "<QHash>";
        includelist << "../../tltypes.h";
        includelist << "../../../types/basic.h";
    });

    cdecl.methods([](QList<MethodDeclarationStatement*>& methodlist) {
        MethodDeclarationStatement* mds = new MethodDeclarationStatement("QString", "constructorName", MethodDeclarationStatement::STATIC);
        mds->addArgument("TLConstructor", "ctor");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "initConstructors", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        methodlist << mds;
    });

    cdecl.fields([](QList<FieldDeclarationStatement*>& fieldlist) {
        FieldDeclarationStatement* fds = new FieldDeclarationStatement("QHash<TLConstructor, QString>", "_constructors", FieldDeclarationStatement::STATIC);
        fieldlist << fds;
    });

    cdef.method("constructorName", [](QString& body) {
        IfStatement ifs;

        ifs.addIf("DecompilerTable::_constructors.isEmpty()", [](QString& body) {
            body += "DecompilerTable::initConstructors();";
        });

        body += ifs.toString() + "\n";
        ifs = IfStatement();

        ifs.addIf("!DecompilerTable::_constructors.contains(ctor)", [](QString& body) {
            body += "qWarning(\"Cannot find 0x%08X\", ctor);\n";
            body += "return QString();";
        });

        body += ifs.toString() + "\n";
        body += "return DecompilerTable::_constructors[ctor];";
    });

    cdef.method("initConstructors", [this](QString& body) {
        this->compileTable(body, this->_schema);
        this->compileTable(body, this->_mtprotoschema);
    });

    cdecl.write();
    cdef.write();
}

void DecompilerGenerator::generateDecompiler()
{
    ClassDeclarationStatement cdecl("MTProtoDecompiler", this->_destinationdir);
    ClassDefinitionStatement cdef(&cdecl, this->_destinationdir);

    cdecl.setBaseClass("MTProtoDecompilerBase");

    cdecl.includes([](QSet<QString>& includelist) {
       includelist << "<functional>";
       includelist << "<QObject>";
       includelist << "<QHash>";
       includelist << "decompilertable.h";
       includelist << "../../../autogenerated/mtproto/mtproto.h";
       includelist << "../../../autogenerated/types/types.h";
       includelist << "../../../mtproto/decompiler/mtprotodecompilerbase.h";
    });

    cdecl.methods([this](QList<MethodDeclarationStatement*>& methodlist) {
        MethodDeclarationStatement* mds = new MethodDeclarationStatement("void", "decompile", MethodDeclarationStatement::VIRTUAL);
        mds->addArgument("int", "dcid");
        mds->addArgument("int", "direction");
        mds->addArgument("TLLong", "messageid");
        mds->addArgument("const QByteArray&", "data");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "doDecompile");
        mds->addArgument("QString&", "result");
        mds->addArgument("MTProtoStream&", "mtstream");
        mds->addArgument("TLLong", "messageid");
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "decompileTLVectorEx");
        mds->addArgument("QString&", "result");
        mds->addArgument("MTProtoStream&", "mtstream");
        mds->addArgument("bool", "isbaretype");
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "initDispatchers", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        methodlist << mds;

        this->compileDispatchersDeclarations(methodlist, this->_schema);
        this->compileDispatchersDeclarations(methodlist, this->_mtprotoschema);
    });

    cdecl.fields([](QList<FieldDeclarationStatement*>& fieldlist) {
        FieldDeclarationStatement* fds = new FieldDeclarationStatement("QHash< TLConstructor, std::function<void(MTProtoDecompiler*, QString&, MTProtoStream&)> >",
                                                                       "_ctordispatcher", FieldDeclarationStatement::STATIC);

        fds->setVisibility(FieldDeclarationStatement::PRIVATE);
        fieldlist << fds;
    });

    cdef.method("decompile", [](QString& body) {
        body += "QString result;\n";
        body += "MTProtoStream mtstream(data);\n";
        body += "this->doDecompile(result, mtstream, messageid);\n\n";

        IfStatement ifs;
        ifs.addIf("result.isEmpty()", [](QString& body) {
            body += "return;";
        });

        body += ifs.toString() + "\n";

        ifs = IfStatement("direction");

        ifs.addIf("==", "MTProtoDecompiler::DIRECTION_IN", [](QString& body) {
            body += "qDebug(\"DC %d IN (%16llx) %s\", dcid, messageid, qUtf8Printable(result));";
        });

        ifs.addIf("==", "MTProtoDecompiler::DIRECTION_OUT", [](QString& body) {
            body += "qDebug(\"DC %d OUT (%16llx) %s\", dcid, messageid, qUtf8Printable(result));";
        });

        body += ifs.toString() + "\n";
    });

    cdef.method("doDecompile", [](QString& body) {
        IfStatement ifs;

        ifs.addIf("MTProtoDecompiler::_ctordispatcher.isEmpty()", [](QString& body) {
            body += "MTProtoDecompiler::initDispatchers();";
        });

        body += ifs.toString() + "\n";
        body += "TLConstructor ctor = mtstream.peekTLConstructor();\n\n";

        ifs = IfStatement("ctor");

        ifs.addIf("==", "TLTypes::Vector", [](QString& body) {
            body += "return;";
        });

        ifs.addIf("==", "TLTypes::BoolTrue", [](QString& body) {
            body += "return;";
        });

        ifs.addIf("==", "TLTypes::BoolFalse", [](QString& body) {
            body += "return;";
        });

        ifs.addIf("!MTProtoDecompiler::_ctordispatcher.contains(ctor)", [](QString& body) {
            body += "qWarning(\"-- (%llx) Invalid constructor: 0x%08x\", messageid, ctor);\n";
            body += "return;";
        });

        body += ifs.toString() + "\n";
        body += "MTProtoDecompiler::_ctordispatcher[ctor](this, result, mtstream);";
    });

    cdef.method("decompileTLVectorEx", [this](QString& body) {
        this->compileTLVectorDefinitions(body);
    });

    cdef.method("initDispatchers", [this](QString& body) {
        this->compileDispatchers(body, this->_schema);
        this->compileDispatchers(body, this->_mtprotoschema);
    });

    this->compileDispatchersDefinitions(&cdef, this->_schema, this->_constructors);
    this->compileDispatchersDefinitions(&cdef, this->_mtprotoschema, this->_mtprotoconstructors);

    cdecl.write();
    cdef.write();
}
