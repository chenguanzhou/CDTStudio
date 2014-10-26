#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTFileSystem
TEMPLATE = lib

INCLUDEPATH += ../../Interfaces
DEFINES += CDTFILESYSTEM_LIBRARY

SOURCES += \
    cdtfilesystem.cpp \
    cdtfileinfo.cpp

HEADERS += cdtfilesystem_global.h \
    cdtfilesystem.h \
    cdtfileinfo.h

FORMS    +=

DESTDIR = ../../lib
DLLDESTDIR = ../../bin

unix {
    QMAKE_CXXFLAGS += -std=c++0x
    target.path = /usr/lib
    INSTALLS += target
    LIBS += -lgdal
    INCLUDEPATH += /usr/include/gdal \
    /usr/local/include/gdal
}
!unix{
    include(../Config/win.pri)
    LIBS += -lgdal_i
}
#log4qt
INCLUDEPATH += ../
INCLUDEPATH += ../log4qt
DEPENDPATH += ../log4qt
LIBS += -L../../lib -llog4qt

