#-------------------------------------------------
#
# Project created by QtCreator 2014-09-13T12:25:24
#
#-------------------------------------------------

QT       = core

TARGET = Otsu
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins
INCLUDEPATH += ../../../Interfaces\

SOURCES += \
    otsu.cpp

HEADERS += \
    otsu.h \
    ../../../Interfaces/cdtautothresholdinterface.h
OTHER_FILES += otsu.json

unix {
    QMAKE_CXXFLAGS += -std=c++0x
    target.path = /usr/lib
    INSTALLS += target
}
