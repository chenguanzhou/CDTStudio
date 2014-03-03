#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       += core gui

TARGET = MSTMethod
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../CDTStudio/plugins

SOURCES += \
    mstmethodinterface.cpp \
    formmst.cpp\
    polygonizer.cpp\
    mstsegmenter.cpp\
    graphkruskal.cpp

HEADERS +=\
            ../../../Interfaces/cdtsegmentationInterface.h \
    mstmethodinterface.h \
    formmst.h\
    polygonizer.h\
    mstsegmenter.h\
    graphkruskal.h\
    cdtbasethread.h

OTHER_FILES += MSTMethod.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    formmst.ui

LIBS += -lgdal_i -lstxxl

#DEFINES += BOOST_THREAD_USE_LIB
