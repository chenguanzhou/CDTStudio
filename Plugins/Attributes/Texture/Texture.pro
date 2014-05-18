#-------------------------------------------------
#
# Project created by QtCreator 2014-03-25T20:07:48
#
#-------------------------------------------------

QT       += core gui sql

TARGET = texture
TEMPLATE = lib
CONFIG += plugin
QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = ../../../bin/Plugins

INCLUDEPATH += ../../../Interfaces

SOURCES += textureinterface.cpp

HEADERS += textureinterface.h\
    ../../../Interfaces/cdtattributesinterface.h
OTHER_FILES += texture.json

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

