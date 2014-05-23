#-------------------------------------------------
#
# Project created by QtCreator 2014-03-14T09:31:00
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Spectral
TEMPLATE = lib
CONFIG += Plugin
QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = ../../../bin/plugins

INCLUDEPATH += ../../../Interfaces

SOURCES += spectralinterface.cpp

HEADERS += spectralinterface.h \
    ../../../Interfaces/cdtattributesinterface.h
OTHER_FILES += Spectral.json

unix{
target.path = /usr/lib
INSTALLS += target
LIBS += -lgdal
INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}
!unix{
include(../../../Tools/Config/win.pri)
LIBS += -lgdal_i
}

