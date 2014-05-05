#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T15:58:23
#
#-------------------------------------------------

QT       += widgets sql xml

TARGET = CDTDialogs
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../../Interfaces

SOURCES +=  dialogdbconnection.cpp\
    dialognewproject.cpp \
    dialognewimage.cpp \
    dialogconsole.cpp \

HEADERS += ../../Interfaces/cdtsegmentationInterface.h \
    dialogdbconnection.h \
    dialognewproject.h \
    dialognewimage.h \
    dialogconsole.h \

FORMS    += dialogdbconnection.ui\
    dialognewproject.ui \
    dialognewimage.ui \
    dialogconsole.ui \

DESTDIR = ../../lib

unix {
    target.path = /usr/lib
    INSTALLS += target
}

