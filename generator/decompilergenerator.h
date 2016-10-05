#ifndef DECOMPILERGENERATOR_H
#define DECOMPILERGENERATOR_H

#include "basicgenerator.h"
#include <QSet>

class DecompilerGenerator : public BasicGenerator
{
    public:
        DecompilerGenerator(const TelegramSchema& schema, const TelegramConstructors& constructors, const QString& destinationdir);
        virtual void generate();

    private:
        void decompileField(const SchemaItem::SchemaField& field, const TelegramSchema &schema, bool* islast, QString &body);
        void compileTable(QString &body, const TelegramSchema &schema);
        void compileDispatchers(QString& body, const TelegramSchema &schema);
        void compileDispatchersDeclarations(QList<MethodDeclarationStatement*>& methodlist, const TelegramSchema &schema);
        void compileDispatchersDefinitions(ClassDefinitionStatement* cdef, const TelegramSchema &schema, const TelegramConstructors &constructors);
        void compileTLVectorDefinitions(QString& body);
        void typeDispatcher(QString& body, const SchemaItem* item, const TelegramSchema &schema, const TelegramConstructors &constructors);
        void functionDispatcher(QString& body, const SchemaItem* item, const TelegramSchema &schema);
        void fieldDispatcher(QString& body, const SchemaItem* item, const TelegramSchema &schema);
        void generateDecompiler();
        void generateTable();

     private:
        QSet<QString> _manualitems;
};

#endif // DECOMPILERGENERATOR_H
