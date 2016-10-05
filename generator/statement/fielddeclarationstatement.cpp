#include "fielddeclarationstatement.h"

FieldDeclarationStatement::FieldDeclarationStatement(const QString &type, const QString &name, int attributes): AbstractStatement(name), _type(type), _attributes(attributes)
{
    this->setVisibility(FieldDeclarationStatement::PRIVATE);
}

bool FieldDeclarationStatement::isStatic() const
{
    return this->_attributes & FieldDeclarationStatement::STATIC;
}

const QString &FieldDeclarationStatement::type() const
{
    return this->_type;
}

const QString &FieldDeclarationStatement::defaultValue() const
{
    return this->_defaultvalue;
}

void FieldDeclarationStatement::setDefaultValue(const QString &v)
{
    this->_defaultvalue = v;
}

FieldDeclarationStatement::~FieldDeclarationStatement()
{

}

QString FieldDeclarationStatement::toString() const
{
    QString s = "\t\t";

    if(this->_attributes == FieldDeclarationStatement::STATIC)
        s += QString("static") + " ";

    s += this->_type + " " + this->_name.toLower() + ";\n";
    return s;
}
