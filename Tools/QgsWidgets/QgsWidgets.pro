TEMPLATE = lib
TARGET = QgsWidgets

QT += xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ../../lib
DLLDESTDIR = ../../bin
DEFINES += QGSWIDGETS_LIBRARY

unix{
QMAKE_CXXFLAGS += -std=c++0x
/usr/include/qgis \
/usr/local/include/qgis

DEFINES += CORE_EXPORT=
DEFINES += GUI_EXPORT=
}
!unix{
include(../Config/win.pri)
}

LIBS += -lqgis_core -lqgis_gui

SOURCES += \
    qgsundowidget.cpp

HEADERS=    \
    qgswidgets_global.h \
    qgsundowidget.h
