#-------------------------------------------------
#
# Project created by QtCreator 2014-09-25T19:41:56
#
#-------------------------------------------------

QT       += network xml xmlpatterns

QT       -= gui

TARGET = CDTTaskClient
TEMPLATE = lib

DESTDIR = ../../../lib
DLLDESTDIR = ../../../bin

DEFINES += CDTTASKCLIENT_LIBRARY

SOURCES += cdttaskclient.cpp

HEADERS += cdttaskclient.h\
        cdttaskclient_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../Global/global.pri)
