#-------------------------------------------------
#
# Project created by QtCreator 2014-03-14T09:31:00
#
#-------------------------------------------------

QT       += core sql

TARGET = Spectral
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += spectralinterface.cpp

HEADERS += spectralinterface.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += Spectral.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
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

