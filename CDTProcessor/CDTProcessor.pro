#-------------------------------------------------
#
# Project created by QtCreator 2014-09-10T16:24:41
#
#-------------------------------------------------

QT       += core network xml xmlpatterns

!win32{
QT       -= gui
CONFIG   += console
}

TARGET = CDTProcessor
DESTDIR = ../bin

TEMPLATE = app


SOURCES += main.cpp \
    cdtprocessorapplication.cpp \
    cdttask.cpp \
    cdttaskmanager.cpp \
    cdttask_pbcdbinary.cpp \
    messagehandler.cpp

HEADERS += \
    cdtprocessorapplication.h \
    cdttask.h \
    cdttaskmanager.h \
    cdttask_pbcdbinary.h \
    messagehandler.h\
    ../Interfaces/cdtattributesinterface.h \
    ../Interfaces/cdtautothresholdinterface.h \
    ../Interfaces/cdtclassifierinterface.h \
    ../Interfaces/cdtpbcddiffinterface.h \
    ../Interfaces/cdtpbcdmergeinterface.h

INCLUDEPATH += \
    ../Interfaces \
    ../Tools\

unix{
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lgdal

INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}
!unix{
include(../Tools/Config/win.pri)
INCLUDEPATH += $(GDAL_ROOT)/include
LIBS += -L$(GDAL_ROOT)/lib  -lgdal_i
}

LIBS += -L../lib -llog4qt

RESOURCES += \
    resource.qrc

