#ifndef SCHEMAPARSER_H
#define SCHEMAPARSER_H

#include <QHash>
#include <QList>
#include "../tokenizer/schematokenizer.h"
#include "abstractparser.h"

class SchemaParser: public AbstractParser
{
    public:
        SchemaParser();
        const TelegramSchema& schema() const;
        const TelegramConstructors constructors() const;
        virtual SchemaParser::ParseResult parse(const QStringList &lines);

    private:
        void elaborateSchema();
        void uncollide(SchemaItem *item);
        void rename(SchemaItem::SchemaField *field);
        void rename(SchemaItem *item);

    private:
        TelegramSchema _schema;
        TelegramConstructors _constructors;
};

#endif // SCHEMAPARSER_H
