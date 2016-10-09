#ifndef QMLGENERATOR_H
#define QMLGENERATOR_H

#include "basicgenerator.h"

class QmlGenerator : public BasicGenerator
{
    public:
        QmlGenerator(const TelegramSchema& schema, const TelegramConstructors& constructors, const QString& destinationdir);
        virtual void generate();
};

#endif // QMLGENERATOR_H
