#include "ifstatement.h"

IfStatement::IfStatement(): AbstractStatement(QString()), _elsebodylinescount(0)
{

}

IfStatement::IfStatement(const QString &name): AbstractStatement(name), _elsebodylinescount(0)
{

}

void IfStatement::addIf(const QString& cmp, const QString& rhs, std::function<void(QString&)> bodyfunc)
{
    this->_ifstatements << ("(" + this->_name + " " + cmp + " " + rhs  + ")");

    QString body;
    bodyfunc(body);

    this->_ifbodylinescount << this->indent(body, 1);
    this->_ifbodies << body;
}

void IfStatement::addIf(const QString &cmpstring, std::function<void (QString &)> bodyfunc)
{
    this->_ifstatements << ("(" + cmpstring  + ")");

    QString body;
    bodyfunc(body);

    this->_ifbodylinescount << this->indent(body, 1);
    this->_ifbodies << body;
}

void IfStatement::setElse(std::function<void(QString &)> bodyfunc)
{
    QString body;
    bodyfunc(body);

    this->_elsebodylinescount = this->indent(body, 1);
    this->_elsebody = body;
}

QString IfStatement::toString() const
{
    QString content;
    int lines = 0;

    for(int i = 0; i < this->_ifstatements.length(); i++)
    {
        lines = this->_ifbodylinescount[i];

        if(content.isEmpty())
            content += "if";
        else
            content += "else if";

        content += this->_ifstatements[i] + "\n";

        if(lines != 1)
            content += "{\n";

        content += this->_ifbodies[i];

        if(lines != 1)
            content += "}\n";
    }

    if(!this->_elsebody.isEmpty())
    {
        lines = this->_elsebodylinescount;
        content += "else\n";

        if(lines != 1)
            content += "{\n";

        content += this->_elsebody;

        if(lines != 1)
            content += "}\n";
    }

    return content;
}
