#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T22:51:27
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Geometry
TEMPLATE = lib
CONFIG += plugin
QMAKE_CXXFLAGS += -std=c++0x


DESTDIR = ../../../bin/Plugins


INCLUDEPATH += ../../../Interfaces

SOURCES += geometryinterface.cpp

HEADERS += geometryinterface.h \
    ../../../Interfaces/cdtattributesinterface.h

OTHER_FILES += Geometry.json

unix{
target.path = /usr/lib
INSTALLS += target
LIBS += -lgdal
INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}
!unix{
LIBS += -lgdal_i
}
