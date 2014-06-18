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

SOURCES += tst_filesystem.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

#log4qt
include(../../Tools/log4qt/log4qt.pri)
