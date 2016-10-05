#include "schemaparser.h"
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDir>
#include <QFile>
#include <QDebug>

SchemaParser::SchemaParser(): AbstractParser()
{

}

const TelegramSchema &SchemaParser::schema() const
{
    return this->_schema;
}

const TelegramConstructors SchemaParser::constructors() const
{
    return this->_constructors;
}

SchemaParser::ParseResult SchemaParser::parse(const QStringList &lines)
{
    if(lines.isEmpty())
        return SchemaParser::SyntaxError;

    SchemaTokenizer tokenizer(this->_mtprotomode);
    SchemaTokenizer::State state;
    QString section;

    for(int i = 0; i < lines.length(); i++)
    {
        SchemaItem* item = NULL;
        const QString& line = lines.at(i);

        if(line.trimmed().startsWith("//"))
            continue;

        this->_lastline = i;
        state = tokenizer.tokenize(line, &item, (section == "functions"));

        if(state == SchemaTokenizer::Section)
        {
            section = item->section();
            delete item;
            continue;
        }

        if(state == SchemaTokenizer::Skip)
            continue;

        if(state == SchemaTokenizer::Error)
            return SchemaParser::SyntaxError;

        if(section.isEmpty())
        {
            if(!this->_mtprotomode)
                continue;

            section = "types";
        }

        if(!this->_schema.contains(section))
            this->_schema[section] = QList<SchemaItem*>();

        this->_schema[section].append(item);

        if(section == "types")
        {
            if(!this->_constructors.contains(item->name()))
                this->_constructors[item->name()] = QList<SchemaItem*>();

            this->_constructors[item->name()].append(item);
        }
    }

    this->elaborateSchema();
    return SchemaParser::OK;
}

void SchemaParser::elaborateSchema()
{
    if(this->_schema.contains("types"))
    {
        foreach(SchemaItem* item, this->_schema["types"])
        {
            this->uncollide(item);
            this->rename(item);
        }
    }

    if(this->_schema.contains("functions"))
    {
        foreach(SchemaItem* item, this->_schema["functions"])
        {
            const SchemaItem::SchemaFields& fields = item->fields();

            for(int i = 0; i < fields.length(); i++)
            {
                SchemaItem::SchemaField* field = item->field(i);
                this->rename(field);
            }
        }
    }
}

void SchemaParser::uncollide(SchemaItem* item)
{
    QHash< QString, QList<SchemaItem::SchemaField*> > collisions;
    QHash< SchemaItem::SchemaField*, SchemaItem*> collisionctor;

    foreach(SchemaItem* ctoritem, this->_constructors[item->name()])
    {
        const SchemaItem::SchemaFields& fields = ctoritem->fields();

        for(int i = 0; i < fields.length(); i++)
        {
            SchemaItem::SchemaField* field = ctoritem->field(i);

            if(!collisions.contains(field->name))
               collisions[field->name] = QList<SchemaItem::SchemaField*>();

            bool ignored = false;

            for(int j = 0; j < collisions[field->name].length(); j++)
                ignored = field->type == collisions[field->name][j]->type;

            if(!ignored)
            {
                collisions[field->name] << field;
                collisionctor[field] = ctoritem;
            }
        }
    }

    QList<QString> keys = collisions.keys();

    foreach(QString key, keys)
    {
        QList<SchemaItem::SchemaField*> collisionlist = collisions[key];

        if(collisionlist.length() < 2)
            continue;

        foreach(SchemaItem::SchemaField* field, collisionlist)
            field->name += "_" +  collisionctor[field]->ctor().toLower();
    }
}

void SchemaParser::rename(SchemaItem::SchemaField *field)
{
    if(field->name == "lat")
        field->name = "latitude";
    else if(field->name == "long")
        field->name = "longitude";
    else if(((field->type == "TLBool") || (field->type == "TLTrue")) && !field->name.startsWith("is", Qt::CaseInsensitive) && !field->name.startsWith("has"))
        field->name = "is_" + field->name;
}

void SchemaParser::rename(SchemaItem *item)
{
    foreach(SchemaItem* ctoritem, this->_constructors[item->name()])
    {
        const SchemaItem::SchemaFields& fields = ctoritem->fields();

        for(int i = 0; i < fields.length(); i++)
        {
            SchemaItem::SchemaField* field = ctoritem->field(i);
            this->rename(field);
        }
    }
}
