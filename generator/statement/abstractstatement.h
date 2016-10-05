#ifndef ABSTRACTSTATEMENT_H
#define ABSTRACTSTATEMENT_H

#include <functional>
#include <QString>
#include <QList>

class AbstractStatement
{
    public:
        enum PropertyType { NO_PROPERTY = 0, GETTER = 1, SETTER = 2 };
        enum Visibility { NO_VISIBILITY = 0, PUBLIC = 1, PRIVATE = 2, PROTECTED = 3 };
        enum Attributes { NO_ATTRIBUTES = 0, CONST = 1, STATIC = 2, VIRTUAL = 4, ABSTRACT = 8, BY_REFERENCE = 16, CONST_BY_REFERENCE = 32 };

    public:
        AbstractStatement(const QString& name, const QString& destinationdir = QString());
        virtual ~AbstractStatement();
        void define(std::function<void(QString&, QString&)> definefunc);
        const QString& name() const;
        void setVisibility(int v);
        int visibility() const;
        virtual void write() const;
        virtual QString toString() const = 0;

    private:
        QByteArray apiGeneratorMark() const;

    protected:
        void doWrite(const QString &ext) const;
        int indent(QString& s, int indent = 1);
        QString firstAttribute(int attributes) const;
        QString lastAttribute(int attributes) const;
        QString visibilityString(int v) const;
        QString camelCase(const QString &s, bool firstupper = false) const;

    protected:
        int _visibility;
        QString _name;
        QString _destinationdir;
        QString _content;
        QList<QString> _definelist;
};

#endif // ABSTRACTSTATEMENT_H
