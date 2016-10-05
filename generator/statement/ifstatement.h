#ifndef IFSTATEMENT_H
#define IFSTATEMENT_H

#include <functional>
#include <QList>
#include "abstractstatement.h"

class IfStatement : public AbstractStatement
{
    public:
        IfStatement();
        IfStatement(const QString& name);
        void addIf(const QString& cmp, const QString& rhs, std::function<void(QString&)> bodyfunc);
        void addIf(const QString& cmpstring, std::function<void(QString&)> bodyfunc);
        void setElse(std::function<void(QString&)> bodyfunc);
        virtual QString toString() const;

    private:
        QList<QString> _ifstatements;
        QList<QString> _ifbodies;
        QList<int> _ifbodylinescount;
        QString _elsebody;
        int _elsebodylinescount;
};

#endif // IFSTATEMENT_H
