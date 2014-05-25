QT       += core gui

TARGET = KNN
TEMPLATE = lib
CONFIG += plugin
QMAKE_CXXFLAGS += -std=c++0x


DESTDIR = ../../../bin/Plugins


INCLUDEPATH += ../../../Interfaces

SOURCES += knninterface.cpp

HEADERS += knninterface.h \
    ../../../Interfaces/cdtclassifierinterface.h

OTHER_FILES += KNN.json

unix{
target.path = /usr/lib
INSTALLS += target
LIBS += -lopencv_core -lopencv_ml
}
!unix{
include(../../../Tools/Config/win.pri)
}
