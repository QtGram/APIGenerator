#ifndef TYPEPARSER_H
#define TYPEPARSER_H

#include <QStringList>
#include <QSet>
#include "../tokenizer/schemaitem.h"
#include "abstractparser.h"

class TypeParser: public AbstractParser
{
    public:
        struct SchemaConstructor {
            QString ns;
            QString name;
            QString value;
            bool isfunction;
        };

        typedef QList<SchemaConstructor> SchemaTypes;

    public:
        TypeParser();
        const QString& layer() const;
        const SchemaTypes& types() const;
        virtual ParseResult parse(const QStringList &lines);

    private:
        SchemaTypes _types;
        QSet<QString> _parsedctors;
        QString _layer;
};

#endif // TYPEPARSER_H
