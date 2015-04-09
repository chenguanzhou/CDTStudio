#-------------------------------------------------
#
# Project created by QtCreator 2014-03-25T20:07:48
#
#-------------------------------------------------

QT       += core gui

TARGET = texture
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += textureinterface.cpp

HEADERS += textureinterface.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += texture.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
target.path = /usr/lib
INSTALLS += target
LIBS += -lgdal
INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}
!unix{
INCLUDEPATH += $(OSGEO4W_ROOT)/include
LIBS += -L$(OSGEO4W_ROOT)/lib
LIBS += -lgdal_i
}

