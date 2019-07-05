QT       += core gui widgets xml

TARGET = PartialSegmentation
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

OTHER_FILES += PartialSegmentation.json

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

win32:CONFIG(release, debug|release): LIBS += -lstxxl
else:win32:CONFIG(debug, debug|release): LIBS += -lstxxld
else:unix: LIBS += -lstxxl

LIBS += -lqgis_core -lqgis_gui

HEADERS += \
    CdtPartialSegmentationMapTool.h \
    PartialSegmentationInterface.h \
    MstPartialSegment.h

SOURCES += \
    CdtPartialSegmentationMapTool.cpp \
    PartialSegmentationInterface.cpp \
    MstPartialSegment.cpp

SOURCES += \
    graphkruskal.cpp

HEADERS +=\
    graphkruskal.h

include(../../../Interfaces/Interfaces.pri)
include(../../../Tools/Config/link2opencv.pri)
DEFINES += _XKEYCHECK_H

