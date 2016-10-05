#include "basicgenerator.h"
#include "statement/classdeclarationstatement.h"
#include <QSet>
#include <QDir>
#include <QDebug>

BasicGenerator::BasicGenerator(const QString &destinationdir): _destinationdir(destinationdir), _mtprotomode(false)
{
    QDir dir;
    dir.mkpath(destinationdir);
}

BasicGenerator::BasicGenerator(const TelegramSchema &schema, const TelegramConstructors& constructors, const QString &destinationdir): _schema(schema), _constructors(constructors), _destinationdir(destinationdir), _mtprotomode(false)
{
    QDir dir;
    dir.mkpath(destinationdir);
}

void BasicGenerator::setMTProtoSchema(const TelegramSchema &schema, const TelegramConstructors &constructors)
{
    this->_mtprotoschema = schema;
    this->_mtprotoconstructors = constructors;
}

void BasicGenerator::setMTProtoMode(bool mtprotomode)
{
    this->_mtprotomode = mtprotomode;
}

SchemaItem *BasicGenerator::fieldToItem(const SchemaItem::SchemaField &field, const TelegramSchema& schema)
{
    if(!this->_schema.contains("types"))
        return NULL;

    foreach(SchemaItem* item, schema["types"])
    {
        if(item->name() == field.type)
            return item;
    }

    return NULL;
}

void BasicGenerator::iterateTypes(const SchemaList &schemalist, std::function<void (const SchemaItem *)> itemproc)
{
    QSet<QString> uniquetypesset;

    foreach(const SchemaItem* item, schemalist)
    {
        if(uniquetypesset.contains(item->name()))
            continue;

        uniquetypesset.insert(item->name());
        itemproc(item);
    }
}

void BasicGenerator::iterateFields(const SchemaItem *item, std::function<void(const SchemaItem::SchemaField &)> fieldproc, bool uniquetypes, bool uniquenames)
{
    QSet<QString> uniquetypesset;
    QSet<QString> uniquenameset;

    foreach(const SchemaItem* ctoritem, this->_constructors[item->name()])
    {
        foreach(const SchemaItem::SchemaField& field, ctoritem->fields())
        {
            if(uniquetypes)
            {
                if(uniquetypesset.contains(field.type))
                    continue;

                uniquetypesset.insert(field.type);
            }

            if(uniquenames)
            {
                if(uniquenameset.contains(field.name))
                    continue;

                uniquenameset.insert(field.name);
            }

            fieldproc(field);
        }
    }
}

QString BasicGenerator::decoratedType(const SchemaItem::SchemaField &field) const
{
    if(IS_FIELD_VECTOR(field))
        return QString("TLVector<%1>").arg(IS_FIELD_BASIC_TYPE(field) ? field.vectortype : (field.vectortype + "*"));

    if(!IS_FIELD_BASIC_TYPE(field))
        return field.type + "*";

    return field.type;
}

QString BasicGenerator::initValue(const SchemaItem::SchemaField &field) const
{
    if(IS_FIELD_BASIC_TYPE(field))
    {
        if((field.type == "TLInt") || (field.type == "TLLong") || (field.type == "TLDouble"))
            return "0";
        else if((field.type == "TLBool") || field.type == "TLTrue")
            return "false";
    }
    else if(!IS_FIELD_VECTOR(field))
        return "NULL";

    return QString();
}
