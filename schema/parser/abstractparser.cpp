#include "abstractparser.h"

AbstractParser::AbstractParser(): _lastline(-1), _mtprotomode(false)
{

}

int AbstractParser::lastLine() const
{
    return this->_lastline;
}

void AbstractParser::setMTProtoMode(bool b)
{
    this->_mtprotomode = b;
}
