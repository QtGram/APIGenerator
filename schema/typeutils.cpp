#include "typeutils.h"
#include <QStringList>

TypeUtils::TypeUtils()
{

}

QString TypeUtils::camelCase(const QString &s, bool firstupper)
{
    if(s.isEmpty())
        return QString();

    QStringList parts = s.split('_', QString::SkipEmptyParts);

    for (int i = 1; i < parts.size(); i++)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    QString res = parts.join("");

    if(firstupper)
        return res.at(0).toUpper() + res.mid(1);

    return res;
}

QString TypeUtils::apiCall(const QString &name)
{
    return name.at(0).toLower() + name.mid(1);
}

QString TypeUtils::fullType(const QString &ns, const QString &type, bool firstupper, bool stringisbytes)
{
    QString t = TypeUtils::cppType(type.startsWith("%") ? type.mid(1) : type, stringisbytes);

    if(ns.isEmpty())
        return TypeUtils::camelCase(t, firstupper);

    return TypeUtils::camelCase(ns, firstupper) + TypeUtils::camelCase(t, true);
}

QString TypeUtils::fullType(const QString &nstype, bool firstupper, bool stringisbytes)
{
    if(nstype.contains("."))
    {
        QStringList sl = nstype.split(".");
        Q_ASSERT(sl.length() == 2);

        return TypeUtils::fullType(sl.first(), sl.last(), firstupper, stringisbytes);
    }

    return TypeUtils::fullType(QString(), nstype, firstupper, stringisbytes);
}

QString TypeUtils::cppType(const QString &type, bool stringisbytes)
{
    if((type == "#") || (type == "int"))
        return "TLInt";

    if(type == "int128")
        return "TLInt128";

    if(type == "int256")
        return "TLInt256";

    if(type == "long")
        return "TLLong";

    if(type == "double")
        return "TLDouble";

    if(type == "true")
        return "TLTrue";

    if(type == "Bool")
        return "TLBool";

    if((type == "string") && !stringisbytes)
        return "TLString";

    if((type == "bytes") || (stringisbytes && (type == "string")))
        return "TLBytes";

    return type;
}

bool TypeUtils::isBareType(const QString &nstype)
{
    if(nstype.contains("."))
    {
        QStringList sl = nstype.split(".");
        Q_ASSERT(sl.length() == 2);

        return sl.last().startsWith("%");
    }

    return nstype.startsWith("%");
}
