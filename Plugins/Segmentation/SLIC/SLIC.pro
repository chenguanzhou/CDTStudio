#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       = core

TARGET = SLIC
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    slicinterface.cpp \
    slic.cpp \
    gdal2opencv.cpp

HEADERS +=\
    slicinterface.h \
    slic.h \
    gdal2opencv.h


include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += SLIC.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix{
LIBS += -lgdal
QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}

!unix{
#stxxl and gdal
INCLUDEPATH += \
    $(OSGEO4W_ROOT)/include 

LIBS += \
    -L$(OSGEO4W_ROOT)/lib

LIBS += -lgdal_i
}

#opencv
include(../../../Tools/Config/link2opencv.pri)


