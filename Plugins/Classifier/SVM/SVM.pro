QT       += core gui

TARGET = SVM
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../../bin/Plugins
INCLUDEPATH += ../../../Interfaces

SOURCES += svminterface.cpp

HEADERS += svminterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += SVM.json

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
