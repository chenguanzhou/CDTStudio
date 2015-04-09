#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       = core

TARGET = MSTMethod
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    mstmethodinterface.cpp \
    graphkruskal.cpp

HEADERS +=\
    mstmethodinterface.h \
    graphkruskal.h


include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += MSTMethod.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix{
LIBS += -lgdal
QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
}

!unix{
#stxxl
INCLUDEPATH += \
    $(STXXL_ROOT)/include \
    $(BOOST_ROOT)

LIBS += \
    -L$(STXXL_ROOT)/lib \
    -L$(BOOST_LIBRARYDIR)

LIBS += -lgdal_i
}

LIBS += -lstxxl

#DEFINES += BOOST_THREAD_USE_LIB
