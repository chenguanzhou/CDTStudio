#-------------------------------------------------
#
# Project created by QtCreator 2014-09-10T16:24:41
#
#-------------------------------------------------

QT       += core network xml
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

TARGET = Processor
DESTDIR = ../bin

TEMPLATE = app


SOURCES += main.cpp \
    cdtprocessorapplication.cpp \
    cdttask.cpp \
    cdttaskmanager.cpp \
    cdttask_pbcdbinary.cpp

HEADERS += \
    cdtprocessorapplication.h \
    cdttask.h \
    cdttaskmanager.h \
    cdttask_pbcdbinary.h

INCLUDEPATH += \
    ../Interfaces \
    ../Tools\

LIBS += -L../lib -llog4qt
