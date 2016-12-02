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
        static QString fullType(const QString& ns, const QString& type, bool firstupper = true, bool stringisbytes = false);
        static QString fullType(const QString& nstype, bool firstupper = true, bool stringisbytes = false);
        static QString cppType(const QString& type, bool stringisbytes);
        static bool isBareType(const QString& nstype);
};

#endif // TYPEUTILS_H
