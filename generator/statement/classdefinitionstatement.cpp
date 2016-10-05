#include "classdefinitionstatement.h"

ClassDefinitionStatement::ClassDefinitionStatement(ClassDeclarationStatement *classdeclaration, const QString &destinationdir): AbstractStatement(classdeclaration->name(), destinationdir), _classdeclaration(classdeclaration)
{

}

ClassDefinitionStatement::~ClassDefinitionStatement()
{
}

void ClassDefinitionStatement::constructor(std::function<void (QString &)> methodproc)
{
    this->method(this->name(), methodproc);
}

void ClassDefinitionStatement::method(const QString& name, std::function<void (QString &)> methodproc)
{
    QString body;
    methodproc(body);

    this->indent(body);
    this->_methodmap[name] = body;
}

void ClassDefinitionStatement::write() const
{
    this->doWrite(".cpp");
}

QString ClassDefinitionStatement::toString() const
{
    QString content = "#include \"" + this->_name.toLower() + ".h\"\n\n";

    foreach(FieldDeclarationStatement* fielddecl, this->_classdeclaration->_fieldlist)
    {
        if(!fielddecl->isStatic())
            continue;

        content += fielddecl->type() + " " + this->_name + "::" + fielddecl->name().toLower();

        if(!fielddecl->defaultValue().isEmpty())
            content += " = " + fielddecl->defaultValue();

        content += ";\n";
    }

    if(!this->_classdeclaration->_fieldlist.isEmpty())
        content += "\n";

    foreach(MethodDeclarationStatement* methoddecl, this->_classdeclaration->_methodlist)
    {
        if(this->_methodmap.contains(methoddecl->name()))
        {
            content += MethodDefinitionStatement(this->_methodmap[methoddecl->name()], methoddecl, this->_classdeclaration).toString() + "\n";
            continue;
        }

        content += MethodDefinitionStatement(QString(), methoddecl, this->_classdeclaration).toString() + "\n";
    }

    return content;
}
