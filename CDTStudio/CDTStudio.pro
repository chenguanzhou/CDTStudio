#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTStudio
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    cdtproject.cpp \
    cdtimagelayer.cpp \
    cdtsegmentationlayer.cpp \
    cdtclassification.cpp \
    cdtattributes.cpp \
    cdtsample.cpp \
    cdtprojecttabwidget.cpp \
    cdtprojectwidget.cpp \
    cdtprojecttreemodel.cpp \
    cdtprojecttreeitem.cpp \
    dialognewproject.cpp \
    dialognewsegmentation.cpp\
    dialognewimage.cpp \
    cdtbaseobject.cpp \
    cdtattributeswidget.cpp\
    recentfilesupervisor.cpp

HEADERS  += \
    ../Interfaces/cdtsegmentationInterface.h \
    mainwindow.h \
    cdtproject.h \
    cdtimagelayer.h \
    cdtsegmentationlayer.h \
    cdtclassification.h \
    cdtattributes.h \
    cdtsample.h \
    cdtprojecttabwidget.h \
    cdtprojectwidget.h \
    cdtprojecttreemodel.h \
    cdtprojecttreeitem.h \
    dialognewproject.h \
    dialognewimage.h\
    dialognewsegmentation.h \
    cdtbaseobject.h \
    cdtpluginloader.h \
    cdtattributeswidget.h\
    recentfilesupervisor.h

FORMS    += mainwindow.ui \
    dialognewproject.ui \
    dialognewsegmentation.ui\
        dialognewimage.ui \
    cdtattributeswidget.ui

INCLUDEPATH += ../Interfaces

RESOURCES += \
    ../resource.qrc

