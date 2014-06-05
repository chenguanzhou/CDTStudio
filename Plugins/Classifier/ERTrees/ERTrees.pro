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
LIBS += -lopencv_core -lopencv_ml
}
!unix{
include(../../../Tools/Config/win.pri)
}
