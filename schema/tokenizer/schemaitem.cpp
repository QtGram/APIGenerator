#include "schemaitem.h"

SchemaItem::SchemaItem(const QString &section): _section(section), _containsflags(false)
{

}

SchemaItem::SchemaItem(const QString &ctor, const QString &name, const QString &value): _ctor(ctor), _name(name), _value(value), _containsflags(false)
{

}

void SchemaItem::addField(const QString &type, const QString &name, quint64 attributes)
{
    this->addField(type, name, QString(), attributes, -1);
}

void SchemaItem::addField(const QString &type, const QString &name, const QString &fieldref, quint64 attributes, int bitno)
{
    if(this->isBasicType(type))
        attributes |= SchemaItem::BASIC_TYPE;

    if(type == "TLTrue")
    {
        this->_containsflags = true;
        attributes |= SchemaItem::TRUE;
    }

    if(!this->_containsflags && (bitno >= 0))
        this->_containsflags = true;

    this->_fields.append({ (attributes & SchemaItem::VECTOR)  ? QString("TLVector<%1>").arg(type) : type,
                           name,
                           fieldref,
                           (attributes & SchemaItem::VECTOR) ? type : QString(),
                           attributes,
                           bitno });
}

const QString &SchemaItem::section() const
{
    return this->_section;
}

const QString &SchemaItem::ctor() const
{
    return this->_ctor;
}

const QString &SchemaItem::name() const
{
    return this->_name;
}

const QString &SchemaItem::value() const
{
    return this->_value;
}

const SchemaItem::SchemaFields &SchemaItem::fields() const
{
    return this->_fields;
}

SchemaItem::SchemaField *SchemaItem::field(int index)
{
    return &(this->_fields[index]);
}

bool SchemaItem::containsFlags() const
{
    return this->_containsflags;
}

bool SchemaItem::isBasicType(const QString &type)
{
    if(type == "TLInt")
        return true;

    if(type == "TLInt128")
        return true;

    if(type == "TLInt256")
        return true;

    if(type == "TLDouble")
        return true;

    if(type == "TLLong")
        return true;

    if(type == "TLTrue")
        return true;

    if(type == "TLBool")
        return true;

    if(type == "TLString")
        return true;

    if(type == "TLBytes")
        return true;

    return false;
}
