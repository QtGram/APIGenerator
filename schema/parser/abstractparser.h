#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H

#include <QStringList>
#include "global.h"

class AbstractParser
{
    public:
        enum ParseResult { OK, NoFile, SyntaxError };

    public:
        AbstractParser();
        int lastLine() const;
        void setMTProtoMode(bool b);
        virtual AbstractParser::ParseResult parse(const QStringList &lines) = 0;

    protected:
        int _lastline;
        bool _mtprotomode;
};

#endif // ABSTRACTPARSER_H
