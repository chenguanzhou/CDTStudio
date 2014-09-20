#-------------------------------------------------
#
# Project created by QtCreator 2014-09-20T10:27:55
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMLSchemaValidator
TEMPLATE = app


SOURCES += main.cpp\
        dialogvalidator.cpp \
    xmlsyntaxhighlighter.cpp

HEADERS  += dialogvalidator.h \
    xmlsyntaxhighlighter.h

FORMS    += dialogvalidator.ui

RESOURCES += \
    resource.qrc
