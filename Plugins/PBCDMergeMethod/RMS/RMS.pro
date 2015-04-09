#-------------------------------------------------
#
# Project created by QtCreator 2014-09-13T12:25:24
#
#-------------------------------------------------

QT       = core

TARGET = RMS
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    rms.cpp

HEADERS += \
    rms.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += rms.json

unix {
    QMAKE_CXXFLAGS += -std=c++0x
    target.path = /usr/lib
    INSTALLS += target
}
