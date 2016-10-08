#ifndef SYNCGENERATOR_H
#define SYNCGENERATOR_H

#include "basicgenerator.h"

class SyncGenerator: public BasicGenerator
{
    public:
        SyncGenerator(const TelegramSchema& schema, const TelegramConstructors &constructors, const QString& destinationdir);
        virtual void generate();

    private:
        bool needsSync(SchemaItem* ctoritem);
        void syncItem(SchemaItem* ctoritem, QString &body);
        void generateSync(SchemaItem* updateitem, QString &body);
};

#endif // SYNCGENERATOR_H
