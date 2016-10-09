#include <QCoreApplication>
#include <QRegularExpression>
#include <QDebug>
#include <QDir>
#include "schema/parser/schemaparser.h"
#include "generator/enumgenerator.h"
#include "generator/syncgenerator.h"
#include "generator/typegenerator.h"
#include "generator/qmlgenerator.h"
#include "generator/apigenerator.h"
#include "generator/decompilergenerator.h"

bool splitSchema(const QString& schemafile, QStringList& mtproto, QStringList& schema)
{
    QFile f(schemafile);

    if(!f.open(QFile::ReadOnly))
        return false;

    QString s = QString::fromUtf8(f.readAll());
    QStringList lines = s.split(QRegularExpression("[\r\n]"), QString::SkipEmptyParts);
    f.close();

    for(int i = 0; i < lines.length(); i++)
    {
        if(lines[i].contains("Main application API"))
        {
            mtproto = lines.mid(0, i - 1);
            schema = lines.mid(i + 1);
            return true;
        }
    }

    return false;
}

bool parse(SchemaParser& sp, const QStringList& lines)
{
    SchemaParser::ParseResult result = sp.parse(lines);

    if(result == SchemaParser::NoFile)
    {
        qDebug() << "File doesn't exists";
        return false;
    }

    if(result == SchemaParser::SyntaxError)
    {
        qDebug() << "Syntax error at line" << sp.lastLine();
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.length() < 3)
    {
        qDebug() << "USAGE: APIGenerator schema.tl destdir";
        return 1;
    }

    QDir destdir(qApp->applicationDirPath() + QDir::separator());
    destdir.setPath(destdir.absoluteFilePath(args.at(2)));

    QStringList mtproto, schema;
    SchemaParser schemasp, mtprotosp;
    mtprotosp.setMTProtoMode(true);

    if(!splitSchema(args.at(1), mtproto, schema))
    {
        qDebug() << "Cannot split MTProto/Schema";
        return 2;
    }

    if(parse(schemasp, schema))
    {
        TypeGenerator typegen(schemasp.schema(), schemasp.constructors(), destdir.absoluteFilePath("types"));
        typegen.generate();

        APIGenerator apigen(schemasp.schema(), schemasp.constructors(), destdir.absolutePath());
        apigen.generate();

        SyncGenerator syncgen(schemasp.schema(), schemasp.constructors(), destdir.absolutePath());
        syncgen.generate();

        QmlGenerator qmlgen(schemasp.schema(), schemasp.constructors(), destdir.absolutePath());
        qmlgen.generate();
    }

    if(parse(mtprotosp, mtproto))
    {
        TypeGenerator typegen(mtprotosp.schema(), mtprotosp.constructors(), destdir.absoluteFilePath("mtproto"));
        typegen.generate();

        APIGenerator apigen(mtprotosp.schema(), mtprotosp.constructors(), destdir.absolutePath());
        apigen.setMTProtoMode(true);
        apigen.setName("MTProtoAPI");
        apigen.generate();
    }

    EnumGenerator enumgen(mtproto, schema, destdir.absolutePath());
    enumgen.generate();

    DecompilerGenerator decgen(schemasp.schema(), schemasp.constructors(), destdir.absoluteFilePath("mtproto/decompiler"));
    decgen.setMTProtoSchema(mtprotosp.schema(), mtprotosp.constructors());
    decgen.generate();

    qDebug() << "OK";
    return a.exec();
}
