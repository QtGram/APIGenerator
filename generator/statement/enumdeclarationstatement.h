#ifndef ENUMDECLARATIONSTATEMENT_H
#define ENUMDECLARATIONSTATEMENT_H

#include "abstractstatement.h"

class EnumDeclarationStatement: public AbstractStatement
{
    public:
        EnumDeclarationStatement(const QString& name, const QString& destinationdir = QString());
        void addEntry(const QString& name, const QString& value);
        virtual void write() const;
        virtual QString toString() const;

    private:
        QString _entries;
};

#endif // ENUMDECLARATIONSTATEMENT_H
