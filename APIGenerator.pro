QT += core
QT -= gui

CONFIG += c++11

TARGET = APIGenerator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    schema/tokenizer/schematokenizer.cpp \
    schema/tokenizer/schemaitem.cpp \
    generator/basicgenerator.cpp \
    generator/statement/abstractstatement.cpp \
    generator/statement/classdeclarationstatement.cpp \
    generator/statement/methoddeclarationstatement.cpp \
    generator/statement/fielddeclarationstatement.cpp \
    generator/statement/propertydeclarationstatement.cpp \
    generator/statement/signalsdeclarationstatements.cpp \
    generator/statement/enumdeclarationstatement.cpp \
    generator/statement/classdefinitionstatement.cpp \
    generator/statement/methoddefinitionstatement.cpp \
    generator/statement/assertstatement.cpp \
    generator/statement/ifstatement.cpp \
    generator/apigenerator.cpp \
    generator/enumgenerator.cpp \
    generator/typegenerator.cpp \
    generator/decompilergenerator.cpp \
    generator/statement/whilestatement.cpp \
    generator/statement/forstatement.cpp \
    generator/statement/masterincludestatement.cpp \
    schema/parser/abstractparser.cpp \
    schema/parser/schemaparser.cpp \
    schema/parser/typeparser.cpp \
    schema/typeutils.cpp \
    generator/syncgenerator.cpp \
    generator/qmlgenerator.cpp

HEADERS += \
    schema/tokenizer/schematokenizer.h \
    schema/tokenizer/schemaitem.h \
    generator/basicgenerator.h \
    generator/statement/abstractstatement.h \
    generator/statement/classdeclarationstatement.h \
    generator/statement/methoddeclarationstatement.h \
    generator/statement/fielddeclarationstatement.h \
    generator/statement/statement.h \
    global.h \
    generator/statement/propertydeclarationstatement.h \
    generator/statement/signalsdeclarationstatements.h \
    generator/statement/enumdeclarationstatement.h \
    generator/statement/classdefinitionstatement.h \
    generator/statement/methoddefinitionstatement.h \
    generator/statement/assertstatement.h \
    generator/statement/ifstatement.h \
    generator/typegenerator.h \
    generator/enumgenerator.h \
    generator/apigenerator.h \
    generator/decompilergenerator.h \
    generator/statement/whilestatement.h \
    generator/statement/forstatement.h \
    generator/statement/masterincludestatement.h \
    schema/parser/abstractparser.h \
    schema/parser/schemaparser.h \
    schema/parser/typeparser.h \
    schema/typeutils.h \
    generator/syncgenerator.h \
    generator/qmlgenerator.h
