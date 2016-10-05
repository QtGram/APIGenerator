#ifndef SCHEMAITEM_H
#define SCHEMAITEM_H

#include <QString>
#include <QList>
#include <QHash>

#define IS_FIELD_ATTRIBUTE_SET(field, bit) (field.attributes & bit)
#define IS_FIELD_BARE_TYPE(field) IS_FIELD_ATTRIBUTE_SET(field, SchemaItem::BARE_TYPE)
#define IS_FIELD_VECTOR(field) IS_FIELD_ATTRIBUTE_SET(field, SchemaItem::VECTOR)
#define IS_FIELD_BASIC_TYPE(field) IS_FIELD_ATTRIBUTE_SET(field, SchemaItem::BASIC_TYPE)
#define IS_FIELD_FLAG(field) IS_FIELD_ATTRIBUTE_SET(field, SchemaItem::FLAG)
#define IS_FIELD_FLAG_KEEPER(field) IS_FIELD_ATTRIBUTE_SET(field, SchemaItem::FLAG_KEEPER)
#define IS_FIELD_TRUE(field) IS_FIELD_ATTRIBUTE_SET(field, SchemaItem::TRUE)

#define BARE_TYPE_STRING(field) QString(IS_FIELD_BARE_TYPE(field) ? "true" : "false")

class SchemaItem
{
    public:
        enum FieldAttributes {
            NO_ATTRIBUTES = 0,
            BARE_TYPE     = 1,
            VECTOR        = 2,
            BASIC_TYPE    = 4,
            FLAG          = 8,
            FLAG_KEEPER   = 16,
            TRUE          = 32,
        };

        struct SchemaField {
            QString type;
            QString name;
            QString fieldref;
            QString vectortype;
            quint64 attributes;
            int bitno;
        };

        typedef QList<SchemaField> SchemaFields;

    public:
        SchemaItem(const QString& section);
        SchemaItem(const QString& constructor, const QString& name, const QString& value);
        void addField(const QString& type, const QString& name, quint64 attributes = SchemaItem::NO_ATTRIBUTES);
        void addField(const QString& type, const QString& name, const QString& fieldref, quint64 attributes, int bitno);
        const QString& section() const;
        const QString& ctor() const;
        const QString& name() const;
        const QString& value() const;
        const SchemaFields &fields() const;
        SchemaField* field(int index);
        bool containsFlags() const;

    private:
        bool isBasicType(const QString& type);

    private:
        QString _section;
        QString _ctor;
        QString _name;
        QString _value;
        SchemaFields _fields;
        bool _containsflags;
};

typedef QList<SchemaItem*> SchemaList;
typedef QHash<QString, SchemaList> TelegramSchema;
typedef QHash<QString, SchemaList> TelegramConstructors;

#endif // SCHEMAITEM_H
