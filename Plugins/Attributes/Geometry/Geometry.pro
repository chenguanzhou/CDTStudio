#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T22:51:27
#
#-------------------------------------------------

QT       += core sql

TARGET = Geometry
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += geometryinterface.cpp

HEADERS += geometryinterface.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += Geometry.json

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

#opencv
include(../../../Tools/Config/link2opencv.pri)
