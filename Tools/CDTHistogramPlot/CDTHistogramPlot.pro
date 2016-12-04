TEMPLATE = lib
TARGET = CDTHistogramPlot
QT += core sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    cdthistogramplot.cpp

HEADERS=    \
            cdthistogramplot_global.h \
    cdthistogramplot.h

DESTDIR = ../../lib
DLLDESTDIR = ../../bin
DEFINES += CDTHISTOGRAMPLOT_LIBRARY

macx{
CONFIG += qwt
}
unix:!macx{
INCLUDEPATH += /usr/include/qwt
LIBS += -lqwt
}
!unix{
include(../Config/win.pri)
LIBS += -lqwt
}

RESOURCES += \
    resource.qrc
