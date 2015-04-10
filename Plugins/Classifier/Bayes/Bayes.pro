QT       += core

TARGET = Bayes
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += bayesinterface.cpp

HEADERS += bayesinterface.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += Bayes.json

unix{
QMAKE_CXXFLAGS += -std=c++0x
target.path = /usr/lib
INSTALLS += target
}
!unix{
INCLUDEPATH += $(OSGEO4W_ROOT)/include
LIBS += -L$(OSGEO4W_ROOT)/lib
}

#opencv
include(../../../Tools/Config/link2opencv.pri)
