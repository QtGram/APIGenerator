#include "methoddefinitionstatement.h"

MethodDefinitionStatement::MethodDefinitionStatement(const QString &body, MethodDeclarationStatement *methoddeclaration, ClassDeclarationStatement *classedeclaration): AbstractStatement(methoddeclaration->name()), _body(body), _methoddeclaration(methoddeclaration), _classdeclaration(classedeclaration)
{

}

QString MethodDefinitionStatement::toString() const
{
    QString content;

    if(!this->_methoddeclaration->_returnval.isEmpty())
        content += this->_methoddeclaration->attributedType(this->_methoddeclaration->_returnval, this->_methoddeclaration->_attributes) + " " ;

    content += this->_classdeclaration->name() + "::" + this->_methoddeclaration->name();

    if(this->_methoddeclaration->_returnval.isEmpty())
    {
        QList<QString> args = this->_methoddeclaration->_arguments;
        args.append("QObject* parent");

        content += "(" + this->_methoddeclaration->arguments(args) + ") " + this->lastAttribute(this->_methoddeclaration->_attributes);

        if(!this->_classdeclaration->baseClass().isEmpty())
            content += ": " + this->_classdeclaration->baseClass() + "(parent)"; // NOTE: Hardcoded is bad
    }
    else
        content += "(" + this->_methoddeclaration->arguments(this->_methoddeclaration->_arguments) + ") " + this->lastAttribute(this->_methoddeclaration->_attributes);

    content += "\n{\n";

    if(this->_classdeclaration->_propertymap.contains(this->name()))
    {
        MethodDeclarationStatement::PropertyType proptype = this->_classdeclaration->_propertymap[this->name()];
        QString fieldname = this->_classdeclaration->_fieldmap[this->name()];

        if(proptype == MethodDeclarationStatement::SETTER)
        {
            content += "\tif(this->_" + fieldname + " == " + fieldname + ")\n";
            content += "\t\treturn;\n\n";

            content += "\tthis->_" + fieldname + " = " + fieldname + ";\n";
            content += "\temit " + this->camelCase(fieldname) + "Changed();\n";
        }
        else if(proptype == MethodDeclarationStatement::GETTER)
            content += "\treturn this->_" + fieldname  + ";\n";
    }
    else
        content += this->_body;

    content += "}\n";
    return content;
}
