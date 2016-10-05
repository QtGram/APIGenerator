#ifndef METHODDEFINITIONSTATEMENT_H
#define METHODDEFINITIONSTATEMENT_H

#include "abstractstatement.h"
#include "classdeclarationstatement.h"
#include "methoddeclarationstatement.h"

class MethodDefinitionStatement : public AbstractStatement
{
    public:
        MethodDefinitionStatement(const QString& body, MethodDeclarationStatement* methoddeclaration, ClassDeclarationStatement* classedeclaration);
        virtual QString toString() const;

    private:
        QString _body;
        MethodDeclarationStatement* _methoddeclaration;
        ClassDeclarationStatement* _classdeclaration;
};

#endif // METHODDEFINITIONSTATEMENT_H
