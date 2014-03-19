#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T22:51:27
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Geometry
TEMPLATE = lib
CONFIG += plugin



DESTDIR = ../../../CDTStudio/plugins


INCLUDEPATH += ../../../Interfaces

SOURCES += geometryinterface.cpp

HEADERS += geometryinterface.h \
    ../../../Interfaces/cdtattributesinterface.h

OTHER_FILES += Geometry.json

LIBS += -lgdal_i

unix {
    target.path = /usr/lib
    INSTALLS += target
}
