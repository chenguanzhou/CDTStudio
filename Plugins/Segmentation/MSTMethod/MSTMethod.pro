#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       += core gui

TARGET = MSTMethod
TEMPLATE = lib
CONFIG += plugin
QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = ../../../bin/Plugins

SOURCES += \
    mstmethodinterface.cpp \
    formmst.cpp\
    polygonizer.cpp\
    mstsegmenter.cpp\
    graphkruskal.cpp

HEADERS +=\
    ../../../Interfaces/cdtsegmentationInterface.h \
    ../../../CDTStudio/cdtbasethread.h \
    mstmethodinterface.h \
    formmst.h\
    polygonizer.h\
    mstsegmenter.h\
    graphkruskal.h


INCLUDEPATH += ../../../Interfaces\
    ../../../CDTStudio

OTHER_FILES += MSTMethod.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    formmst.ui

LIBS += -lstxxl

unix{
LIBS += -lgdal

INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}
!unix{
LIBS += -lgdal_i
}

#DEFINES += BOOST_THREAD_USE_LIB
