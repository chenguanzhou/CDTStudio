#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       += sql xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTDialogs
TEMPLATE = lib
#CONFIG += staticlib

INCLUDEPATH += ../../Interfaces
DEFINES += CDTDIALOGS_LIBRARY

SOURCES +=  dialogdbconnection.cpp\
    dialognewproject.cpp \
    dialognewimage.cpp \
    dialogconsole.cpp \

HEADERS += \
    cdtdialogs_global.h \
    dialogdbconnection.h \
    dialognewproject.h \
    dialognewimage.h \
    dialogconsole.h \
    ../../Interfaces/cdtsegmentationinterface.h

FORMS    += dialogdbconnection.ui\
    dialognewproject.ui \
    dialognewimage.ui \
    dialogconsole.ui \

DESTDIR = ../../lib
DLLDESTDIR = ../../bin

unix {
    target.path = /usr/lib
    INSTALLS += target
}
