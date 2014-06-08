QT       += core gui

TARGET = KNN
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins
INCLUDEPATH += ../../../Interfaces

SOURCES += knninterface.cpp

HEADERS += knninterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += KNN.json

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
