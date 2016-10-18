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
        includelist << "types/updatesstate.h";
        includelist << "../config/telegramconfig.h";
    });

    cdecl.methods([](QList<MethodDeclarationStatement*>& methodlist) {
        MethodDeclarationStatement* mds = new MethodDeclarationStatement("void", "syncUpdate", MethodDeclarationStatement::STATIC);
        mds->addArgument("Update*", "u");
        mds->addArgument("UpdatesState*", "clientstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncUpdates", MethodDeclarationStatement::STATIC);
        mds->addArgument("Updates*", "u");
        mds->addArgument("UpdatesState*", "clientstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncState", MethodDeclarationStatement::STATIC);
        mds->addArgument("UpdatesState*", "serverstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncPts", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        mds->addArgument("TLInt", "pts");
        mds->addArgument("UpdatesState*", "clientstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncQts", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        mds->addArgument("TLInt", "qts");
        mds->addArgument("UpdatesState*", "clientstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncDate", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        mds->addArgument("TLInt", "date");
        mds->addArgument("UpdatesState*", "clientstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncSeq", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        mds->addArgument("TLInt", "seq");
        mds->addArgument("UpdatesState*", "clientstate");
        methodlist << mds;

        mds = new MethodDeclarationStatement("void", "syncUnreadCount", MethodDeclarationStatement::STATIC);
        mds->setVisibility(MethodDeclarationStatement::PRIVATE);
        mds->addArgument("TLInt", "unreadcount");
        mds->addArgument("UpdatesState*", "clientstate");
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

    cdef.method("syncState", [this](QString& body) {
        body += "UpdatesState* clientstate = TelegramConfig_clientState;\n\n";
        body += "ClientSyncManager::syncPts(serverstate->pts(), clientstate);\n";
        body += "ClientSyncManager::syncQts(serverstate->qts(), clientstate);\n";
        body += "ClientSyncManager::syncDate(serverstate->date(), clientstate);\n";
        body += "ClientSyncManager::syncSeq(serverstate->seq(), clientstate);\n";
        body += "ClientSyncManager::syncUnreadCount(serverstate->unreadCount(), clientstate);\n";
    });

    cdef.method("syncPts", [this](QString& body) {
        IfStatement ifs("pts");
        ifs.addIf("<=", "clientstate->pts()", [](QString& body) { body += "return;"; });

        body += ifs.toString() + "\n";
        body += "clientstate->setPts(pts);\n";
    });

    cdef.method("syncQts", [this](QString& body) {
        IfStatement ifs("qts");
        ifs.addIf("<=", "clientstate->qts()", [](QString& body) { body += "return;"; });

        body += ifs.toString() + "\n";
        body += "clientstate->setQts(qts);\n";
    });

    cdef.method("syncDate", [this](QString& body) {
        IfStatement ifs("date");
        ifs.addIf("<=", "clientstate->date()", [](QString& body) { body += "return;"; });

        body += ifs.toString() + "\n";
        body += "clientstate->setDate(date);\n";
    });

    cdef.method("syncSeq", [this](QString& body) {
        body += "clientstate->setSeq(seq); // NOTE: How to handle 'seq'?\n";
    });

    cdef.method("syncUnreadCount", [this](QString& body) {
        IfStatement ifs("unreadcount");
        ifs.addIf("<=", "clientstate->unreadCount()", [](QString& body) { body += "return;"; });

        body += ifs.toString() + "\n";
        body += "clientstate->setUnreadCount(unreadcount);\n";
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

        body += "ClientSyncManager::sync" + TypeUtils::camelCase(schemafield.name, true) + "(u->" + schemafield.name + "(), clientstate);\n";
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
