#ifndef WHILESTATEMENT_H
#define WHILESTATEMENT_H

#include <functional>
#include "abstractstatement.h"

class WhileStatement: public AbstractStatement
{
    public:
        WhileStatement();
        WhileStatement(const QString& name);
        void setCondition(const QString& cmp, const QString& rhs, std::function<void(QString&)> bodyproc);
        void setCondition(const QString& condition, std::function<void(QString&)> bodyproc);
        virtual QString toString() const;

    private:
        QString _condition;
        QString _body;
        int _bodylines;
};

#endif // WHILESTATEMENT_H
