#include "typeparser.h"
#include <QRegularExpression>
#include <QDebug>

TypeParser::TypeParser(): AbstractParser()
{
    this->_layer = "0";
}

const QString &TypeParser::layer() const
{
    return this->_layer;
}

const TypeParser::SchemaTypes &TypeParser::types() const
{
    return this->_types;
}

TypeParser::ParseResult TypeParser::parse(const QStringList &lines)
{
    this->_types.clear();

    if(lines.isEmpty())
        return TypeParser::SyntaxError;

    bool functionsection = false;

    for(int i = 0; i < lines.length(); i++)
    {
        const QString& line = lines.at(i);
        this->_lastline = i;

        if(line.trimmed().isEmpty())
            continue;

        QRegularExpressionMatch match = QRegularExpression("\\-\\-\\-([a-z]+)\\-\\-\\-").match(line);

        if(match.hasMatch())
            functionsection = (match.captured(1) == "functions");

        match = QRegularExpression("([0-9a-zA-Z_\\.]+)#([0-9a-f]+)\\s*[^=]+\\s*=\\s*[a-zA-Z0-9_\\.]+").match(line);

        if(!match.hasMatch())
            continue;

        SchemaConstructor ctor;
        ctor.value = match.captured(2);

        if(this->_parsedctors.contains(ctor.value))
            continue;

        this->_parsedctors << ctor.value;

        if(match.captured(1).contains("."))
        {
            ctor.ns = match.captured(1).split(".").first();
            ctor.name = match.captured(1).split(".").last();
        }
        else
            ctor.name = match.captured(1);

        ctor.isfunction = functionsection;
        this->_types << ctor;
    }

    if(!this->_mtprotomode)
    {
        QRegularExpression regex("LAYER ([0-9]+)");
        QRegularExpressionMatch match = regex.match(lines.last());

        if(match.hasMatch())
            this->_layer = match.captured(1);
        else
            qWarning() << "Layer level not found";
    }

    return TypeParser::OK;
}
