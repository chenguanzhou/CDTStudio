#-------------------------------------------------
#
# Project created by QtCreator 2014-03-14T09:31:00
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Spectral
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../CDTStudio/plugins

SOURCES += spectralinterface.cpp

HEADERS += spectralinterface.h
OTHER_FILES += Spectral.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}
