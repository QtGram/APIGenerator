#ifndef METHODDECLARATIONSTATEMENT_H
#define METHODDECLARATIONSTATEMENT_H

#include <QList>
#include <QHash>
#include "abstractstatement.h"

class MethodDeclarationStatement: public AbstractStatement
{
    public:
        MethodDeclarationStatement(const QString& name);
        MethodDeclarationStatement(const QString& returnval, const QString& name, int attributes = AbstractStatement::NO_ATTRIBUTES);
        ~MethodDeclarationStatement();
        void addArgument(const QString& type, const QString& name, const QString& defaultval, int attributes = AbstractStatement::NO_ATTRIBUTES);
        void addArgument(const QString& type, const QString& name, int attributes = AbstractStatement::NO_ATTRIBUTES);
        virtual QString toString() const;

    private:
        QString attributedType(const QString& type, int attributes) const;
        QString arguments(const QList<QString>& arguments) const;

    private:
        QList<QString> _arguments;
        QHash<QString, QString> _defaultvalues;
        int _attributes;
        QString _returnval;

    friend class MethodDefinitionStatement;
};

#endif // METHODDECLARATIONSTATEMENT_H
