#ifndef SCHEMATOKENIZER_H
#define SCHEMATOKENIZER_H

#include <QString>
#include "schemaitem.h"

class SchemaTokenizer
{
    public:
        enum State { OK, Skip, Section, Error };

    public:
        SchemaTokenizer(bool mtprotomode);
        State tokenize(const QString& line, SchemaItem** item, bool infunctionsection);

    private:
        State tokenizeSection(const QString& line, SchemaItem** item);
        State tokenizeFields(const QString& fieldlist, SchemaItem** item);

    private:
        bool _mtprotomode;
};

#endif // SCHEMATOKENIZER_H
