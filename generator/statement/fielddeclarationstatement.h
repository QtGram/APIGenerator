#ifndef FIELDDECLARATIONSTATEMENT_H
#define FIELDDECLARATIONSTATEMENT_H

#include "abstractstatement.h"

class FieldDeclarationStatement: public AbstractStatement
{
    public:
        FieldDeclarationStatement(const QString& type, const QString& name, int attributes = FieldDeclarationStatement::NO_ATTRIBUTES);
        virtual ~FieldDeclarationStatement();
        bool isStatic() const;
        const QString& type() const;
        const QString& defaultValue() const;
        void setDefaultValue(const QString& v);
        virtual QString toString() const;

    private:
        QString _type;
        QString _defaultvalue;
        int _attributes;
};

#endif // FIELDDECLARATIONSTATEMENT_H
