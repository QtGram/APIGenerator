#include "masterincludestatement.h"
#include <QDir>

MasterIncludeStatement::MasterIncludeStatement(const QString &destinationdir): AbstractStatement(QDir(destinationdir).dirName().toLower(), destinationdir)
{

}

void MasterIncludeStatement::addInclude(const QString &include)
{
    if(include.startsWith("#include"))
        this->_includelist << (include + "\n");
    else if(include.startsWith("<") && include.endsWith(">"))
        this->_includelist << ("#include " + include + "\n");
    else
        this->_includelist << ("#include \"" + include.toLower() + "\"\n");
}

void MasterIncludeStatement::write()
{
    this->doWrite(".h");
}

QString MasterIncludeStatement::toString() const
{
    QString content;

    foreach(const QString& include, this->_includelist)
        content.append(include);

    return content;
}
