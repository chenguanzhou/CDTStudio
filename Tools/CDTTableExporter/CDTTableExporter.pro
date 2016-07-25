#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       = core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = CDTTableExporter
TEMPLATE = lib

DEFINES += CDTTABLEEXPORTER_LIBRARY

SOURCES += \ 
    cdttableexporter.cpp

HEADERS += \ 
    cdttableexporter.h \
    cdttableexporter_global.h

FORMS    +=

DESTDIR = ../../lib
DLLDESTDIR = ../../bin

unix {
    target.path = /usr/lib
    INSTALLS += target
}
!unix {
    CONFIG += qaxcontainer
}

