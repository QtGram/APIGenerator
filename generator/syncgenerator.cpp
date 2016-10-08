#include "syncgenerator.h"
#include "../schema/typeutils.h"

SyncGenerator::SyncGenerator(const TelegramSchema &schema, const TelegramConstructors& constructors, const QString &destinationdir): BasicGenerator(schema, constructors, destinationdir)
{

}

void SyncGenerator::generate()
{
    ClassDeclarationStatement cdecl("ClientSyncManager", this->_destinationdir);
    ClassDefinitionStatement cdef(&cdecl, this->_destinationdir);

    cdecl.includes([](QSet<QString>& includelist) {
        includelist << "<QObject>";
        includelist << "tltypes.h";
        includelist << "types/updates.h";
        includelist << "types/update.h";
        includelist << "types/updatesstate.h";
    });

    cdecl.methods([](QList<MethodDeclarationStatement*>& methodlist) {
        MethodDeclarationStatement* mds = new MethodDeclarationStatement("void", "syncUpdate", MethodDeclarationStatement::STATIC);
        mds->addArgument("Update*", "u");
        mds->addArgument("UpdatesState*", "updatestate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncUpdates", MethodDeclarationStatement::STATIC);
        mds->addArgument("Updates*", "u");
        mds->addArgument("UpdatesState*", "updatestate");
        methodlist << mds;
    });

    cdef.method("syncUpdate", [this](QString& body) {
        foreach(SchemaItem* item, this->_schema["types"]) {
            if(item->name() == "Update") {
                this->generateSync(item, body);
                break;
            }
        }
    });

    cdef.method("syncUpdates", [this](QString& body) {
        foreach(SchemaItem* item, this->_schema["types"]) {
            if(item->name() == "Updates") {
                this->generateSync(item, body);
                break;
            }
        }
    });

    cdecl.write();
    cdef.write();
}

bool SyncGenerator::needsSync(SchemaItem *ctoritem)
{
    foreach(const SchemaItem::SchemaField& schemafield, ctoritem->fields())
    {
        if(schemafield.name == "pts")
            return true;

        if(schemafield.name == "qts")
            return true;

        if(schemafield.name == "seq")
            return true;

        if(schemafield.name == "date")
            return true;
    }

    return false;
}

void SyncGenerator::syncItem(SchemaItem *ctoritem, QString& body)
{
    foreach(const SchemaItem::SchemaField& schemafield, ctoritem->fields())
    {
        bool isupdatefield = (schemafield.name == "pts") || (schemafield.name == "qts") ||
                             (schemafield.name == "seq") || (schemafield.name == "date");

        if(!isupdatefield)
            continue;

        body += "updatestate->set" + TypeUtils::camelCase(schemafield.name, true) + "(u->" + schemafield.name + "());\n";
    }
}

void SyncGenerator::generateSync(SchemaItem *updateitem, QString& body)
{
    IfStatement ifs("u->constructorId()");

    foreach (SchemaItem* ctoritem, this->_constructors[updateitem->name()])
    {
        if(!this->needsSync(ctoritem))
            continue;

        ifs.addIf("==", "TLTypes::" + ctoritem->ctor(), [this, ctoritem](QString& body) {
            this->syncItem(ctoritem, body);
        });

    }

    body += ifs.toString();
}
