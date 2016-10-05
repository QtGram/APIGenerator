#ifndef TYPEUTILS_H
#define TYPEUTILS_H

#include <QString>

class TypeUtils
{
    private:
        TypeUtils();

    public:
        static QString camelCase(const QString &s, bool firstupper = false);
        static QString apiCall(const QString& name);
        static QString fullType(const QString& ns, const QString& type, bool firstupper = true);
        static QString fullType(const QString& nstype, bool firstupper = true);
        static QString cppType(const QString& type);
        static bool isBareType(const QString& nstype);
};

#endif // TYPEUTILS_H
