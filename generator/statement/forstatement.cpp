#include "forstatement.h"

ForStatement::ForStatement(const QString &name, const QString &condition, const QString &op): AbstractStatement(name), _condition(condition), _op(op), _bodylinescount(0)
{

}

void ForStatement::setBody(std::function<void(QString &)> bodyfunc)
{
    QString body;
    bodyfunc(body);

    this->_bodylinescount = this->indent(body, 1);
    this->_body = body;
}

QString ForStatement::toString() const
{
    QString content = "for(int ";

    content += this->_name + " = 0; ";
    content += this->_name + " < " + this->_condition + "; ";
    content += this->_name + this->_op;
    content += ")\n";

    if(this->_bodylinescount != 1)
        content += "{\n";

    content += this->_body;

    if(this->_bodylinescount != 1)
        content += "}\n";

    return content;
}
