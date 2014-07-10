TARGET = log4qt
TEMPLATE = lib

DESTDIR = ../../lib
DLLDESTDIR = ../../bin

DEFINES += LOG4QT_LIBRARY

HEADERS += \
    log4qt_global.h

include(log4qt.pri)

