#-------------------------------------------------
#
# Project created by QtCreator 2014-07-20T08:38:49
#
#-------------------------------------------------

QT       += widgets

TARGET = CDTClassifierAssessmentWidget
TEMPLATE = lib

DEFINES += CDTCLASSIFIERASSESSMENTWIDGET_LIBRARY

SOURCES += \
    cdtclassifierassessmentform.cpp

HEADERS +=\
    cdtclassifierassessmentwidget_global.h \
    cdtclassifierassessmentform.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
!unix {
    include(../Config/win.pri)
}

FORMS += \
    cdtclassifierassessmentform.ui

DESTDIR = ../../lib
DLLDESTDIR = ../../bin

#opencv
include(../Config/link2opencv.pri)
