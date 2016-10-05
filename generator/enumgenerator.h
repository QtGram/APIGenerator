#ifndef ENUMGENERATOR_H
#define ENUMGENERATOR_H

#include <QSet>
#include "statement/statement.h"
#include "basicgenerator.h"
#include "../schema/parser/typeparser.h"

class EnumGenerator : public BasicGenerator
{
    public:
        EnumGenerator(const QStringList& mtproto, const QStringList& schema, const QString& destinationdir);
        virtual void generate();

    private:
        void enumerate(EnumDeclarationStatement* enumstmt, const TypeParser::SchemaTypes &types);

    private:
        void generateTLTypes();

   private:
        const QStringList& _mtproto;
        const QStringList& _schema;
        QSet<QString> _names;
};

#endif // ENUMGENERATOR_H
