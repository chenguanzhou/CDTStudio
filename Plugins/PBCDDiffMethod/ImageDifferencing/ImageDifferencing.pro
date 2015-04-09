#-------------------------------------------------
#
# Project created by QtCreator 2014-09-13T12:25:24
#
#-------------------------------------------------

QT       = core

TARGET = ImageDifferencing
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    imagedifferencing.cpp

HEADERS += \
    imagedifferencing.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += ImageDifferencing.json

unix {
INCLUDEPATH += /usr/include/gdal \
            /usr/local/include/gdal \
target.path = /usr/lib
INSTALLS += target
LIBS += -lgdal
}
!unix{
INCLUDEPATH += $(OSGEO4W_ROOT)/include
LIBS += -L$(OSGEO4W_ROOT)/lib
LIBS += -lgdal_i
}
