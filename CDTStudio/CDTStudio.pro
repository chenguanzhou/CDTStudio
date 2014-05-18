#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql xml svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTStudio
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x
CONFIG+=precompile_header
PRECOMPILED_HEADER = stable.h

DESTDIR = ../bin

SOURCES += main.cpp\
        mainwindow.cpp \
    cdtproject.cpp \
    cdtimagelayer.cpp \
    cdtsegmentationlayer.cpp \
    cdtclassification.cpp \
    cdtprojecttabwidget.cpp \
    cdtprojectwidget.cpp \
    cdtprojecttreemodel.cpp \
    cdtprojecttreeitem.cpp \    
    cdtbaseobject.cpp \
    recentfilesupervisor.cpp \    
    cdtattributegenerator.cpp \
    cdtmaptoolselecttrainingsamples.cpp \
    dialognewsegmentation.cpp\    
    dialoggenerateattributes.cpp \    
    dialognewclassification.cpp \
    cdtattributedockwidget.cpp \
    cdtsampledockwidget.cpp


HEADERS  += \    
    mainwindow.h \
    cdtproject.h \
    cdtimagelayer.h \
    cdtsegmentationlayer.h \
    cdtclassification.h \
    cdtprojecttabwidget.h \
    cdtprojectwidget.h \
    cdtprojecttreemodel.h \
    cdtprojecttreeitem.h \    
    cdtbaseobject.h \
    cdtpluginloader.h \
    recentfilesupervisor.h \
    ../Interfaces/cdtattributesinterface.h \    
    cdtattributegenerator.h \
    cdtbasethread.h \
    cdtmaptoolselecttrainingsamples.h \
    dialognewsegmentation.h \
    dialoggenerateattributes.h \    
    dialognewclassification.h \
    cdtattributedockwidget.h \
    cdtsampledockwidget.h


FORMS    += mainwindow.ui \            
    dialognewsegmentation.ui\
    dialoggenerateattributes.ui \    
    dialognewclassification.ui \
    cdtattributedockwidget.ui \
    cdtsampledockwidget.ui

INCLUDEPATH += ../Interfaces \
../Tools/CDTDialogs

DEPENDPATH += ../tools/CDTDialogs

LIBS += -L../lib -lCDTDialogs \
 -lstxxl -lqgis_core -lqgis_gui -lqgis_analysis -lqgis_networkanalysis

unix{
LIBS += -lgdal -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_imgproc -lgomp

INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
INCLUDEPATH += /usr/include/qgis \
/usr/local/include/qgis \

DEFINES += CORE_EXPORT=
DEFINES += GUI_EXPORT=
}
!unix{
DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)
LIBS += -lgdal_i
}


RESOURCES += \
    ../resource.qrc

