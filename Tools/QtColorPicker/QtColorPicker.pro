TEMPLATE = lib
TARGET = QtColorPicker

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    qtcolorpicker.cpp

HEADERS=    \
    qtcolorpicker_global.h \
    qtcolorpicker.h

DESTDIR = ../../lib
DLLDESTDIR = ../../bin
DEFINES += QTCOLORPICKER_LIBRARY

