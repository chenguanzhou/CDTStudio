QT       += core gui

TARGET = AutoSVM
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

INCLUDEPATH += ../../../Interfaces

SOURCES += autosvminterface.cpp

HEADERS += autosvminterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += AutoSVM.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
target.path = /usr/lib
INSTALLS += target
}
!unix{
include(../../../Tools/Config/win.pri)
}

#opencv
include(../../../Tools/Config/link2opencv.pri)
