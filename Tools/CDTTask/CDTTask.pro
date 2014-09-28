#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T14:44:02
#
#-------------------------------------------------

QT       += network xml xmlpatterns

QT       -= gui

TARGET = CDTTask
TEMPLATE = lib

DEFINES += CDTTASK_LIBRARY

SOURCES += cdttaskclient.cpp

HEADERS += cdttaskclient.h\
        cdttask_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
