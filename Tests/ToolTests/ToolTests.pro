#-------------------------------------------------
#
# Project created by QtCreator 2014-06-18T14:56:56
#
#-------------------------------------------------

QT       += widgets testlib

TARGET = tst_filesystem
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    ../../Tools/CDTFileSystem

LIBS += -L../../lib -lCDTFileSystem

unix {
    LIBS += -lgdal
    INCLUDEPATH += /usr/include/gdal \
    /usr/local/include/gdal
}
!unix{
    include(../../Tools/Config/win.pri)
    LIBS += -lgdal_i
}

SOURCES += tst_filesystem.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

#log4qt
INCLUDEPATH += ../../Tools/
DEPENDPATH += ../../Tools/log4qt
LIBS += -L../../lib -llog4qt

