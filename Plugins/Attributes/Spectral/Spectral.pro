#-------------------------------------------------
#
# Project created by QtCreator 2014-03-14T09:31:00
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Spectral
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/plugins

INCLUDEPATH += ../../../Interfaces

SOURCES += spectralinterface.cpp

HEADERS += spectralinterface.h \
    ../../../Interfaces/cdtattributesinterface.h
OTHER_FILES += Spectral.json

LIBS += -lgdal_i

unix {
    target.path = /usr/lib
    INSTALLS += target
}
