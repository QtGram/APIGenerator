#include "whilestatement.h"

WhileStatement::WhileStatement(): AbstractStatement(QString()), _bodylines(0)
{

}

WhileStatement::WhileStatement(const QString &name): AbstractStatement(name), _bodylines(0)
{

}

void WhileStatement::setCondition(const QString &cmp, const QString &rhs, std::function<void (QString &)> bodyproc)
{
    this->_condition = this->_name + " " + cmp + " " + rhs;

    bodyproc(this->_body);
    this->_bodylines = this->indent(this->_body, 1);
}

void WhileStatement::setCondition(const QString &condition, std::function<void (QString &)> bodyproc)
{
    this->_condition = condition;

    bodyproc(this->_body);
    this->_bodylines = this->indent(this->_body, 1);
}

QString WhileStatement::toString() const
{
    QString content = "while(" + this->_condition + ")\n";

    if(this->_bodylines != 1)
        content += "{\n";

    content += this->_body;

    if(this->_bodylines != 1)
        content += "}\n";

    return content;
}
