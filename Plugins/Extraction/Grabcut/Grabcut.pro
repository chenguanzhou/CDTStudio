QT       += core gui widgets xml

TARGET = Grabcut
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

OTHER_FILES += Grabcut.json

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
include(../../../Tools/Config/win.pri)
LIBS += -lgdal_i
}

LIBS += -lqgis_core -lqgis_gui

HEADERS += \
    cdtgrabcutmaptool.h \
    grabcutinterface.h

SOURCES += \
    cdtgrabcutmaptool.cpp \
    grabcutinterface.cpp

include(../../../Interfaces/Interfaces.pri)
include(../../../Tools/Config/link2opencv.pri)
DEFINES += _XKEYCHECK_H
