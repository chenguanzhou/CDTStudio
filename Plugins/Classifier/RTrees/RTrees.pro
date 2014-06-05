QT       += core gui

TARGET = RTrees
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../../bin/Plugins
INCLUDEPATH += ../../../Interfaces

SOURCES += rtreesinterface.cpp

HEADERS += rtreesinterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += RTrees.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
target.path = /usr/lib
INSTALLS += target
LIBS += -lopencv_core -lopencv_ml
}
!unix{
include(../../../Tools/Config/win.pri)
}
