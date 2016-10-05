#ifndef PROPERTYDECLARATIONSTATEMENT_H
#define PROPERTYDECLARATIONSTATEMENT_H

#include "abstractstatement.h"

class PropertyDeclarationStatement: public AbstractStatement
{
    public:
        PropertyDeclarationStatement(const QString& type, const QString& name);
        bool isBasicType() const;
        bool isVector() const;
        const QString& type() const;
        void setIsBasicType(bool b);
        void setIsVector(bool b);
        virtual QString toString() const;

    private:
        QString _type;
        bool _isbasictype;
        bool _isvector;
};

#endif // PROPERTYSTATEMENT_H
