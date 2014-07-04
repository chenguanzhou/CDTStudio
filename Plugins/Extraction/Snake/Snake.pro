QT       += core

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
}
!unix{
include(../../../Tools/Config/win.pri)
LIBS += -lgdal_i
}

HEADERS += \
    ../../../Interfaces/cdtextractioninterface.h \
    StatisticSnake.h \
    snakeinterface.h\


SOURCES += \
    StatisticSnake.cpp \
    snakeinterface.cpp
