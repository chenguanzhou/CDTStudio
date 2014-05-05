#-------------------------------------------------
#
# Project created by QtCreator 2014-03-25T20:07:48
#
#-------------------------------------------------

QT       += core gui sql

TARGET = texture
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/plugins

INCLUDEPATH += ../../../Interfaces

SOURCES += textureinterface.cpp

HEADERS += textureinterface.h\
    ../../../Interfaces/cdtattributesinterface.h
OTHER_FILES += texture.json

LIBS += -lgdal_i

unix {
    target.path = /usr/lib
    INSTALLS += target
}
