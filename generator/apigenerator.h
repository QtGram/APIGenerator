#ifndef APIGENERATOR_H
#define APIGENERATOR_H

#include "basicgenerator.h"

class APIGenerator : public BasicGenerator
{
    public:
        APIGenerator(const TelegramSchema& schema, const TelegramConstructors& constructors, const QString& destinationdir);
        void setName(const QString& name);
        virtual void generate();

    private:
        void compileFlags(const SchemaItem::SchemaFields& fields, QString& body);
        void writeAPIArgument(const SchemaItem::SchemaField &field, QString& body);

    private:
        QString _name;
};

#endif // APIGENERATOR_H
