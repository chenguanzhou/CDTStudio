QT       += core gui

TARGET = ANN
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

INCLUDEPATH += ../../../Interfaces

SOURCES += anninterface.cpp

HEADERS += anninterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += ANN.json

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
