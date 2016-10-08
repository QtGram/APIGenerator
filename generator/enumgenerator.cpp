#include "enumgenerator.h"
#include "../schema/typeutils.h"
#include <QDebug>

EnumGenerator::EnumGenerator(const QStringList &mtproto, const QStringList &schema, const QString &destinationdir): BasicGenerator(destinationdir), _mtproto(mtproto), _schema(schema)
{

}

void EnumGenerator::generate()
{
    this->generateTLTypes();
}

void EnumGenerator::enumerate(EnumDeclarationStatement *enumstmt, const TypeParser::SchemaTypes &types)
{
    foreach(const TypeParser::SchemaConstructor& ctor, types)
    {
        QString name = TypeUtils::fullType(ctor.ns, ctor.name, !ctor.isfunction);

        if(this->_names.contains(name))
            continue;

        this->_names << name;
        enumstmt->addEntry(name, "0x" + ctor.value);
    }
}

void EnumGenerator::generateTLTypes()
{
    EnumDeclarationStatement enumstmt("TLTypes", this->_destinationdir);
    TypeParser tp;
    LAMBDA_POINTER_D(TypeParser, tp);

    tp.setMTProtoMode(true);

    if(tp.parse(this->_mtproto) == TypeParser::OK)
        this->enumerate(&enumstmt, tp.types());
    else
        qDebug() << "MTProto parser error at line:" << tp.lastLine();

    tp.setMTProtoMode(false);

    if(tp.parse(this->_schema) == TypeParser::OK)
        this->enumerate(&enumstmt, tp.types());
    else
        qDebug() << "Schema parser error at line:" << tp.lastLine();

    enumstmt.define([LAMBDA_POINTER_V(tp)](QString& key, QString& value) {
        key = "TELEGRAM_API_LAYER";
        value = LAMBDA_POINTER_V(tp)->layer();
    });

    enumstmt.write();
}
