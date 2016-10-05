#ifndef MASTERINCLUDESTATEMENT_H
#define MASTERINCLUDESTATEMENT_H

#include <QStringList>
#include "abstractstatement.h"

class MasterIncludeStatement : public AbstractStatement
{
    public:
        MasterIncludeStatement(const QString& destinationdir);
        void addInclude(const QString& include);
        virtual void write();
        virtual QString toString() const;

    private:
        QStringList _includelist;
};

#endif // MASTERINCLUDESTATEMENT_H
