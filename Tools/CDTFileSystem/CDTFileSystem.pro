#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
