#ifndef CLASSDECLARATIONSTATEMENT_H
#define CLASSDECLARATIONSTATEMENT_H

#include <functional>
#include <QList>
#include <QHash>
#include <QSet>
#include "abstractstatement.h"
#include "methoddeclarationstatement.h"
#include "fielddeclarationstatement.h"
#include "propertydeclarationstatement.h"
#include "enumdeclarationstatement.h"

class ClassDeclarationStatement: public AbstractStatement
{
    public:
        ClassDeclarationStatement(const QString& name, const QString &destinationdir);
        virtual ~ClassDeclarationStatement();
        const QString& baseClass();
        void setBaseClass(const QString& baseclass);
        void includes(std::function<void(QSet<QString>&)> includeproc);
        void properties(std::function<void(QList<PropertyDeclarationStatement*>&)> propertiesproc);
        void enums(std::function<void(QList<EnumDeclarationStatement*>&)> enumproc);
        void methods(std::function<void( QList<MethodDeclarationStatement*>&) > methodproc);
        void fields(std::function<void( QList<FieldDeclarationStatement*>&) > fieldproc);
        void emitters(std::function<void(QList<QString>&)> emitsproc);
        virtual void write() const;
        virtual QString toString() const;

    private:
        QList<MethodDeclarationStatement*> _methodlist;
        QList<FieldDeclarationStatement*> _fieldlist;
        QHash<QString, ClassDeclarationStatement::PropertyType> _propertymap;
        QHash<QString, QString> _fieldmap;
        QSet<QString> _includeset;
        QString _baseclass;
        QString _properties;
        QString _enums;
        QString _methods;
        QString _fields;
        QString _signals;
        int _lastmethodvisibility;
        int _lastfieldvisibility;
        int _lastenumvisibility;

    friend class ClassDefinitionStatement;
    friend class MethodDefinitionStatement;
};

#endif // CLASSDECLARATIONSTATEMENT_H
