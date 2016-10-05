#include "enumdeclarationstatement.h"

EnumDeclarationStatement::EnumDeclarationStatement(const QString &name, const QString &destinationdir): AbstractStatement(name, destinationdir)
{

}

void EnumDeclarationStatement::addEntry(const QString &name, const QString &value)
{
    if(this->_destinationdir.isEmpty())
    {
        this->_entries += QString("\t\t\t%1 = %2,\n").arg(name, value);
        return;
    }

    this->_entries += QString("\t\t%1 = %2,\n").arg(name, value);
}

void EnumDeclarationStatement::write() const
{
    this->doWrite(".h");
}

QString EnumDeclarationStatement::toString() const
{
    QString content;

    if(!this->_destinationdir.isEmpty())
    {
        content = "namespace " + this->_name + "\n";
        content += "{\n";
        content += "\tenum " + this->_name + "\n";
        content += "\t{\n";
    }
    else
        content = "\t\tenum " + this->_name + " {\n";

    content += this->_entries;

    if(!this->_destinationdir.isEmpty())
    {
        content += "\t};\n";
        content += "}\n";
    }
    else
        content += "\t\t};\n";

    return content;
}
