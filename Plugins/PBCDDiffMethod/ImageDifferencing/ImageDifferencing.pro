#-------------------------------------------------
#
# Project created by QtCreator 2014-09-13T12:25:24
#
#-------------------------------------------------

QT       = core

TARGET = ImageDifferencing
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins
INCLUDEPATH += ../../../Interfaces\

SOURCES += \
    imagedifferencing.cpp

HEADERS += \
    imagedifferencing.h \
    ../../../Interfaces/cdtpbcddiffinterface.h
OTHER_FILES += ImageDifferencing.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}
