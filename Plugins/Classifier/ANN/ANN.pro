QT       += core

TARGET = ANN
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += anninterface.cpp

HEADERS += anninterface.h

include(../../../Interfaces/Interfaces.pri)

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
