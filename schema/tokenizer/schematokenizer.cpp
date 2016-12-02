#include "schematokenizer.h"
#include "../typeutils.h"
#include <QRegularExpression>
#include <QDebug>

SchemaTokenizer::SchemaTokenizer(bool mtprotomode): _mtprotomode(mtprotomode)
{
    if(!this->_mtprotomode)
        return;

    this->_fieldisbytes << "pq";
    this->_fieldisbytes << "p";
    this->_fieldisbytes << "q";
    this->_fieldisbytes << "encrypted_answer";
    this->_fieldisbytes << "encrypted_data";
    this->_fieldisbytes << "dh_prime";
    this->_fieldisbytes << "g_a";
    this->_fieldisbytes << "g_b";
}

SchemaTokenizer::State SchemaTokenizer::tokenize(const QString &line, SchemaItem **item, bool infunctionsection)
{
    if(line.trimmed().isEmpty() || line.startsWith("vector") || line.contains("!X"))
        return SchemaTokenizer::Skip;

    if(line.startsWith("---"))
        return this->tokenizeSection(line, item);

    QRegularExpressionMatch match = QRegularExpression("([0-9a-zA-Z_\\.]+)#([0-9a-f]+)\\s*([^=]+)\\s*=\\s*([a-zA-Z0-9_\\.]+)").match(line);

    if(!match.hasMatch())
        return SchemaTokenizer::Error;

    *item = new SchemaItem(TypeUtils::fullType(match.captured(1), !infunctionsection), TypeUtils::fullType(match.captured(4)), match.captured(2));

    QString fieldlist = match.captured(3).trimmed();

    if(!fieldlist.isEmpty())
        return this->tokenizeFields(fieldlist, item);

    return SchemaTokenizer::OK;
}

SchemaTokenizer::State SchemaTokenizer::tokenizeSection(const QString &line, SchemaItem **item)
{
    QRegularExpressionMatch rm = QRegularExpression("\\-\\-\\-([a-z]+)\\-\\-\\-").match(line);

    if(!rm.hasMatch())
        return SchemaTokenizer::Error;

    *item = new SchemaItem(rm.captured(1));
    return SchemaTokenizer::Section;
}

SchemaTokenizer::State SchemaTokenizer::tokenizeFields(const QString &fieldlist, SchemaItem **item)
{
    QStringList fieldpairs = fieldlist.split(" ");

    foreach(const QString& fieldpair, fieldpairs)
    {
        QStringList field = fieldpair.split(":");
        int extrattributes = SchemaItem::NO_ATTRIBUTES;

        if(field.length() != 2)
            return SchemaTokenizer::Error;

        if(field.last() == "#")
            extrattributes |= SchemaItem::FLAG_KEEPER;

        // Check FIELD.NN?Vector<IDENTIFIER>
        QRegularExpressionMatch rm = QRegularExpression("([a-zA-Z0-9_]+)\\.([0-9]+)\\?(?:vector|Vector)<([%a-zA-Z0-9_\\.]+)>").match(field.last());

        if(rm.hasMatch())
        {
            quint64 attributes = SchemaItem::VECTOR | SchemaItem::FLAG;

            if(TypeUtils::isBareType(rm.captured(3)))
                attributes |= SchemaItem::BARE_TYPE;

            (*item)->addField(TypeUtils::fullType(rm.captured(3), true, this->_fieldisbytes.contains(field.first())),
                              field.first(), rm.captured(1), attributes | extrattributes, rm.captured(2).toInt());
            continue;
        }

        // Check vector<IDENTIFIER>
        rm = QRegularExpression("(?:vector|Vector)<([%a-zA-Z0-9_\\.]+)>").match(field.last());

        if(rm.hasMatch())
        {
            quint64 attributes = SchemaItem::VECTOR;

            if(TypeUtils::isBareType(rm.captured(1)))
                attributes |= SchemaItem::BARE_TYPE;

            (*item)->addField(TypeUtils::fullType(rm.captured(1), true, this->_fieldisbytes.contains(field.first())),
                              field.first(), attributes | extrattributes);
            continue;
        }

        // Check FIELD.NN?TYPE
        rm = QRegularExpression("([a-zA-Z0-9_]+)\\.([0-9]+)\\?([\\.a-zA-Z0-9_]+)").match(field.last());

        if(rm.hasMatch())
        {
            (*item)->addField(TypeUtils::fullType(rm.captured(3), true, this->_fieldisbytes.contains(field.first())),
                              field.first(), rm.captured(1), SchemaItem::FLAG | extrattributes, rm.captured(2).toInt());
            continue;
        }

        (*item)->addField(TypeUtils::fullType(field.last(), true, this->_fieldisbytes.contains(field.first())),
                          field.first(), extrattributes);
    }

    return SchemaTokenizer::OK;
}
