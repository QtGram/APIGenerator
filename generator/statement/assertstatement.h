#ifndef ASSERTSTATEMENT_H
#define ASSERTSTATEMENT_H

#include "abstractstatement.h"
#include <QList>

class AssertStatement : public AbstractStatement
{
    public:
        AssertStatement(const QString& name);
        void addAssertion(const QString& assertion);
        virtual QString toString() const;

    private:
        QList<QString> _assertions;
};

#endif // ASSERTSTATEMENT_H
