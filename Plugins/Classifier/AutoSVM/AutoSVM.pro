QT       += core gui

TARGET = AutoSVM
TEMPLATE = lib
CONFIG += plugin
QMAKE_CXXFLAGS += -std=c++0x


DESTDIR = ../../../bin/Plugins


INCLUDEPATH += ../../../Interfaces

SOURCES += autosvminterface.cpp

HEADERS += autosvminterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += AutoSVM.json

unix{
target.path = /usr/lib
INSTALLS += target
LIBS += -lopencv_core -lopencv_ml
}
!unix{
include(../../../Tools/Config/win.pri)
}
