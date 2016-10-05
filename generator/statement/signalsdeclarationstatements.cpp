#include "signalsdeclarationstatements.h"

SignalsDeclarationStatements::SignalsDeclarationStatements(const QString &name): AbstractStatement(name)
{

}

QString SignalsDeclarationStatements::toString() const
{
    return QString("\t\tvoid %1Changed();\n").arg(this->camelCase(this->_name));
}
