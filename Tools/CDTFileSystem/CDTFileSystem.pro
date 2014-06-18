#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       = core sql

TARGET = CDTFileSystem
TEMPLATE = lib

INCLUDEPATH += ../../Interfaces
DEFINES += CDTFILESYSTEM_LIBRARY

SOURCES += \
    cdtfilesystem.cpp

HEADERS += cdtfilesystem_global.h \
    cdtfilesystem.h

FORMS    +=

DESTDIR = ../../lib
DLLDESTDIR = ../../bin

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#log4qt
include(../log4qt/log4qt.pri)
