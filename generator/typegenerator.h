#ifndef TYPEGENERATOR_H
#define TYPEGENERATOR_H

#include "basicgenerator.h"
#include <QHash>

class TypeGenerator: public BasicGenerator
{
    public:
        explicit TypeGenerator(const TelegramSchema& schema, const TelegramConstructors& constructors, const QString& destinationdir);
        virtual void generate();

    private:
        void generateInclude();
        void compileFlags(const SchemaItem* item, QString& ifbody);
        void compileReadConstructor(const SchemaItem* ctoritem, QString& ifbody);
        void compileWriteConstructor(const SchemaItem* ctoritem, QString& body);
        void readField(const SchemaItem::SchemaField& field, QString& body);
        void writeField(const SchemaItem::SchemaField& field, QString& body);

    private:
        QHash<QString, QString> _manualitems;
};

#endif // TYPEGENERATOR_H
