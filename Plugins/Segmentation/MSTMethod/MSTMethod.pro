#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:20:42
#
#-------------------------------------------------

QT       = core

TARGET = MSTMethod
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../../../bin/Plugins

SOURCES += \
    mstmethodinterface.cpp \
    graphkruskal.cpp

HEADERS +=\
    mstmethodinterface.h \
    graphkruskal.h


include(../../../Interfaces/Interfaces.pri)

OTHER_FILES += MSTMethod.json

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

win32:CONFIG(release, debug|release): LIBS += -lstxxl
else:win32:CONFIG(debug, debug|release): LIBS += -lstxxld
else:unix: LIBS += -lstxxl

message($$INCLUDEPATH)

#DEFINES += BOOST_THREAD_USE_LIB
