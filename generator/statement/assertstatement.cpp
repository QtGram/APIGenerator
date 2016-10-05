#include "assertstatement.h"

AssertStatement::AssertStatement(const QString &name): AbstractStatement(name)
{

}

void AssertStatement::addAssertion(const QString &assertion)
{
    this->_assertions << ("(" + this->_name + " == " + assertion + ")");
}

QString AssertStatement::toString() const
{
    QString content = "Q_ASSERT(";

    for(int i = 0; i < this->_assertions.length(); i++)
    {
        if(i > 0)
            content += " ||\n\t ";

        content += this->_assertions[i];
    }

    content += ");";
    return content;
}
