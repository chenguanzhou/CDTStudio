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

SOURCES += mstsegmenter.cpp \
    formparams.cpp

HEADERS += mstsegmenter.h\
            ../../../Interfaces/cdtsegmentationInterface.h \
    formparams.h
OTHER_FILES += MSTMethod.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    formparams.ui

LIBS += -lgdal_i
