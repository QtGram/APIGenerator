#ifndef FORSTATEMENT_H
#define FORSTATEMENT_H

#include <functional>
#include "abstractstatement.h"

class ForStatement : public AbstractStatement
{
    public:
        ForStatement(const QString& name, const QString& condition, const QString& op);
        void setBody(std::function<void(QString&)> bodyfunc);
        virtual QString toString() const;

    private:
        QString _condition;
        QString _op;
        QString _body;
        int _bodylinescount;
};

#endif // FORSTATEMENT_H
