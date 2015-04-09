QT       += core

TARGET = RTrees
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../../bin/Plugins
SOURCES += rtreesinterface.cpp

HEADERS += rtreesinterface.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += RTrees.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
target.path = /usr/lib
INSTALLS += target
}

#opencv
include(../../../Tools/Config/link2opencv.pri)
