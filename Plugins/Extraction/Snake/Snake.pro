QT       += core gui xml

TARGET = Snake
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

INCLUDEPATH += ../../../Interfaces\

OTHER_FILES += Snake.json

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
include(../../../Tools/Config/win.pri)
LIBS += -lgdal_i
}

LIBS += -lqgis_core -lqgis_gui

HEADERS += \
    ../../../Interfaces/cdtextractioninterface.h \
    StatisticSnake.h \
    snakeinterface.h\
    cdtsnakemaptool.h


SOURCES += \
    StatisticSnake.cpp \
    snakeinterface.cpp \
    cdtsnakemaptool.cpp
