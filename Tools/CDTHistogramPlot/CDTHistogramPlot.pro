TEMPLATE = lib
TARGET = CDTHistogramPlot
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    cdthistogramplot.cpp

HEADERS=    \
            cdthistogramplot_global.h \
    cdthistogramplot.h

DESTDIR = ../../lib
DLLDESTDIR = ../../bin
DEFINES += CDTHISTOGRAMPLOT_LIBRARY

LIBS += -lqwt
!unix{
include(../Config/win.pri)
}
