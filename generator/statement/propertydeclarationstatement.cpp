#include "propertydeclarationstatement.h"

PropertyDeclarationStatement::PropertyDeclarationStatement(const QString &type, const QString &name): AbstractStatement(name), _type(type), _isbasictype(true), _isvector(false)
{

}

bool PropertyDeclarationStatement::isBasicType() const
{
    return this->_isbasictype;
}

bool PropertyDeclarationStatement::isVector() const
{
    return this->_isvector;
}

const QString &PropertyDeclarationStatement::type() const
{
    return this->_type;
}

void PropertyDeclarationStatement::setIsBasicType(bool b)
{
    this->_isbasictype = b;
}

void PropertyDeclarationStatement::setIsVector(bool b)
{
    this->_isvector = b;
}

QString PropertyDeclarationStatement::toString() const
{
    return QString("\tQ_PROPERTY(%1 %2 READ %2 WRITE set%3 NOTIFY %2Changed)\n").arg(this->_type, this->camelCase(this->_name), this->camelCase(this->_name, true));
}
