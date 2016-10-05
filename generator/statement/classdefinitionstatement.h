#ifndef CLASSDEFINITIONSTATEMENT_H
#define CLASSDEFINITIONSTATEMENT_H

#include "abstractstatement.h"
#include "classdeclarationstatement.h"
#include "methoddefinitionstatement.h"

class ClassDefinitionStatement : public AbstractStatement
{
    public:
        ClassDefinitionStatement(ClassDeclarationStatement* classdeclaration, const QString& destinationdir);
        virtual ~ClassDefinitionStatement();
        void constructor(std::function<void(QString &)> methodproc);
        void method(const QString &name, std::function<void(QString &)> methodproc);
        virtual void write() const;
        virtual QString toString() const;

    private:
        ClassDeclarationStatement* _classdeclaration;
        QHash<QString, QString> _methodmap;
};

#endif // CLASSDEFINITIONSTATEMENT_H
