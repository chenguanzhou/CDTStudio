QT       += core gui

TARGET = ERTrees
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../../bin/Plugins
INCLUDEPATH += ../../../Interfaces

SOURCES += ertreesinterface.cpp

HEADERS += ertreesinterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += ERTrees.json

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
