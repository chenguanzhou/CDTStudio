#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       += core xml

TARGET = Overlay
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    overlay.cpp

HEADERS += overlay.h

include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += Overlay.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix{
LIBS += -lgdal
QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
/usr/include/qgis \
/usr/local/include/qgis

DEFINES += CORE_EXPORT=
DEFINES += GUI_EXPORT=
}
!unix{
#QGIS
INCLUDEPATH += $(QGIS_ROOT)/include\
                $(OSGEO4W_ROOT)/include
LIBS += -L$(QGIS_ROOT)/lib\
                -L$(OSGEO4W_ROOT)/lib
DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)

LIBS += -lgdal_i
}

LIBS += -lqgis_core

