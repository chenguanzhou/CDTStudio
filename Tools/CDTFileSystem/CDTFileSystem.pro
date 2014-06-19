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
    LIBS += -lgdal
    INCLUDEPATH += /usr/include/gdal \
    /usr/local/include/gdal
}
!unix{
    include(../../Tools/Config/win.pri)
    LIBS += -lgdal_i
}
#log4qt
include(../log4qt/log4qt.pri)

#quazip
INCLUDEPATH += ../quazip
LIBS += -L../../lib -lquazip
