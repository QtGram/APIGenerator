#include "methoddeclarationstatement.h"

MethodDeclarationStatement::MethodDeclarationStatement(const QString &name): AbstractStatement(name), _attributes(MethodDeclarationStatement::NO_ATTRIBUTES)
{

}

MethodDeclarationStatement::MethodDeclarationStatement(const QString &returnval, const QString &name, int attributes): AbstractStatement(name), _attributes(attributes), _returnval(returnval)
{

}

MethodDeclarationStatement::~MethodDeclarationStatement()
{

}

void MethodDeclarationStatement::addArgument(const QString &type, const QString &name, const QString &defaultval, int attributes)
{
    if(defaultval.isEmpty())
    {
        this->_arguments.append(this->attributedType(type, attributes) + " " + name);
        return;
    }

    QString arg = this->attributedType(type, attributes) + " " + name;
    this->_defaultvalues[arg] = defaultval;
    this->_arguments.append(arg);
}

void MethodDeclarationStatement::addArgument(const QString &type, const QString &name, int attributes)
{
    this->addArgument(type, name, QString(), attributes);
}

QString MethodDeclarationStatement::toString() const
{
    if(this->_returnval.isEmpty()) // Constructor type
    {
        QList<QString> args = this->_arguments;
        args.append("QObject* parent = 0");

        return "\t\texplicit " + this->_name + "(" + this->arguments(args) + ");\n";
    }

    QString s, firstattribute = this->firstAttribute(this->_attributes), lastattribute = this->lastAttribute(this->_attributes);

    if(!firstattribute.isEmpty())
        s += firstattribute + " ";

    s += this->attributedType(this->_returnval, this->_attributes) + " " + this->_name + "(" + this->arguments(this->_arguments) + ")";

    if(!lastattribute.isEmpty())
        s += " " + lastattribute;

    s += ";\n";
    return "\t\t" + s;
}

QString MethodDeclarationStatement::attributedType(const QString &type, int attributes) const
{
    if(attributes & MethodDeclarationStatement::CONST_BY_REFERENCE)
        return "const " + type + "&";

    if(attributes & MethodDeclarationStatement::BY_REFERENCE)
        return type + "&";

    return type;
}

QString MethodDeclarationStatement::arguments(const QList<QString> &arguments) const
{
    QString s;

    foreach(QString argument, arguments)
    {
        if(!s.isEmpty())
            s += ", ";

        s += argument;

        if(this->_defaultvalues.contains(argument))
            s += " = " + this->_defaultvalues[argument];
    }

    return s;
}
