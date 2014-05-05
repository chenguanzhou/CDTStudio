#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql xml svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTStudio
TEMPLATE = app

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
    cdtattributeswidget.cpp\
    recentfilesupervisor.cpp \    
    cdtattributegenerator.cpp \
    cdtmaptoolselecttrainingsamples.cpp \
    cdttrainingsamplesform.cpp\    
    dialognewsegmentation.cpp\    
    dialoggenerateattributes.cpp \    
    dialognewclassification.cpp


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
    cdtattributeswidget.h\
    recentfilesupervisor.h \
    ../Interfaces/cdtattributesinterface.h \    
    cdtattributegenerator.h \
    cdtbasethread.h \
    cdtmaptoolselecttrainingsamples.h \
    cdttrainingsamplesform.h\    
    dialognewsegmentation.h \
    dialoggenerateattributes.h \    
    dialognewclassification.h


FORMS    += mainwindow.ui \            
    cdtattributeswidget.ui \    
    cdttrainingsamplesform.ui\
    dialognewsegmentation.ui\
    dialoggenerateattributes.ui \    
    dialognewclassification.ui

INCLUDEPATH += ../Interfaces \
../tools/CDTDialogs

LIBS += -L../lib -lCDTDialogs \
 -lstxxl -lqgis_core -lqgis_gui -lqgis_analysis -lqgis_networkanalysis

unix{
LIBS += -lgdal -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_imgproc
}
!unix{
DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)
LIBS += -lgdal_i
}


RESOURCES += \
    ../resource.qrc

