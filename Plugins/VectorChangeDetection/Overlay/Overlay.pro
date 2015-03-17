#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       += core

TARGET = Overlay
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    overlay.cpp

HEADERS +=\
    ../../../Interfaces/cdtvectorchangedetectioninterface.h \
    overlay.h


INCLUDEPATH += ../../../Interfaces\
    ../../../CDTStudio

OTHER_FILES += Overlay.json

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
include(../../../Tools/Config/win.pri)
LIBS += -lgdal_i
}

