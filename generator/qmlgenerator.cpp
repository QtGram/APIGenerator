#include "qmlgenerator.h"

QmlGenerator::QmlGenerator(const TelegramSchema &schema, const TelegramConstructors &constructors, const QString &destinationdir): BasicGenerator(schema, constructors, destinationdir)

{

}

void QmlGenerator::generate()
{
    ClassDeclarationStatement cdecl("TelegramQml", this->_destinationdir);
    ClassDefinitionStatement cdef(&cdecl, this->_destinationdir);

    cdecl.includes([](QSet<QString>& includelist) {
        includelist << "types/types.h";
        includelist << "../objects/telegramqmlbase.h";
    });

    cdecl.methods([](QList<MethodDeclarationStatement*>& methodlist) {
        MethodDeclarationStatement* mds = new MethodDeclarationStatement("void", "initialize", MethodDeclarationStatement::STATIC);
        mds->addArgument("QString", "uri", MethodDeclarationStatement::CONST_BY_REFERENCE);

        methodlist << mds;
    });

    cdef.method("initialize", [this](QString& body) {
        body += "TelegramQmlBase::initialize(uri);\n\n";

        if(!this->_schema.contains("types"))
            return;

        QSet<QString> registerednames;

        // Skip list
        registerednames << "TLBool";
        registerednames << "True";
        registerednames << "Error";
        registerednames << "Null";

        foreach(SchemaItem* item, this->_schema["types"]) {
            if(registerednames.contains(item->name()))
                continue;

            registerednames << item->name();
            body += "REGISTER_TYPE(" + item->name() + "*);\n";
            body += "REGISTER_OBJECT(" + item->name() + ");\n\n";
        }
    });

    cdef.write();
    cdecl.write();
}
