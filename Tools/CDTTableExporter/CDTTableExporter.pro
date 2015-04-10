#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       = core sql

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


