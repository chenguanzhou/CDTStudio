TEMPLATE = lib
TARGET = CDTHistogramPlot
QT += core sql svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    cdthistogramplot.cpp

HEADERS=    \
            cdthistogramplot_global.h \
    cdthistogramplot.h

DESTDIR = ../../lib
DLLDESTDIR = ../../bin
DEFINES += CDTHISTOGRAMPLOT_LIBRARY QWT_DLL

macx{
CONFIG += qwt
}

INCLUDEPATH += ../qwt/src

LIBS += -L../../lib -lqwt

RESOURCES += \
    resource.qrc
