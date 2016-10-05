#ifndef SIGNALSDECLARATIONSTATEMENTS_H
#define SIGNALSDECLARATIONSTATEMENTS_H

#include "abstractstatement.h"

class SignalsDeclarationStatements: public AbstractStatement
{
    public:
        SignalsDeclarationStatements(const QString& name);
        virtual QString toString() const;
};

#endif // SIGNALSDECLARATIONSTATEMENTS_H
