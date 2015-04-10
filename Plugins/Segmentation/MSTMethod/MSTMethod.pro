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
#stxxl and gdal
INCLUDEPATH += \
    $(OSGEO4W_ROOT)/include \
    $(STXXL_ROOT)/include \
    $(BOOST_ROOT)

LIBS += \
    -L$(OSGEO4W_ROOT)/lib \
    -L$(STXXL_ROOT)/lib \
    -L$(BOOST_LIBRARYDIR)

LIBS += -lgdal_i
}

INCLUDEPATH += \
    $(OSGEO4W_ROOT)/include \
    $(QGIS_ROOT)/include \
    $(STXXL_ROOT)/include \
    $(QWT_ROOT)/include \
    $(BOOST_ROOT)

LIBS += \
    -L$(OSGEO4W_ROOT)/lib \
    -L$(QGIS_ROOT)/lib \
    -L$(STXXL_ROOT)/lib \
    -L$(QWT_ROOT)/lib \
    -L$(BOOST_LIBRARYDIR)

LIBS += -lstxxl

#DEFINES += BOOST_THREAD_USE_LIB
