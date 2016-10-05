#ifndef BASICGENERATOR_H
#define BASICGENERATOR_H

#define NOT_IMPLEMENTED(res) res += "// NOTE: NOT IMPLEMENTED\n"
#define NOT_IMPLEMENTED_EX(res, s) res += QString("// NOTE: NOT IMPLEMENTED %1\n").arg(s)
#define LAMBDA_POINTER_D(t, v) t* p##v = &v
#define LAMBDA_POINTER_V(v) p##v

#include <functional>
#include <QString>
#include "../schema/tokenizer/schemaitem.h"
#include "statement/statement.h"
#include "global.h"

class BasicGenerator
{
    public:
        BasicGenerator(const QString& destinationdir);
        BasicGenerator(const TelegramSchema& schema, const TelegramConstructors& constructors, const QString& destinationdir);
        void setMTProtoSchema(const TelegramSchema& schema, const TelegramConstructors& constructors);
        void setMTProtoMode(bool mtprotomode);
        virtual void generate() = 0;

    protected:
        SchemaItem* fieldToItem(const SchemaItem::SchemaField& field, const TelegramSchema &schema);
        void iterateTypes(const SchemaList& schemalist, std::function<void(const SchemaItem*)> itemproc);
        void iterateFields(const SchemaItem* item, std::function<void(const SchemaItem::SchemaField &)> fieldproc, bool uniquetypes = true, bool uniquenames = false);
        QString decoratedType(const SchemaItem::SchemaField &field) const;
        QString initValue(const SchemaItem::SchemaField& field) const;

    protected:
        TelegramSchema _schema;
        TelegramConstructors _constructors;
        TelegramSchema _mtprotoschema;
        TelegramConstructors _mtprotoconstructors;
        QString _destinationdir;
        bool _mtprotomode;
};

#endif // BASICGENERATOR_H
