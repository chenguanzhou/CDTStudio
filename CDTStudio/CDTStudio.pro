#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql xml svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.1
TARGET = CDTStudio
TEMPLATE = app
DESTDIR = ../bin

#PRECOMPILED HEADER
CONFIG+=precompile_header qwt
PRECOMPILED_HEADER = stable.h

SOURCES += main.cpp\
    mainwindow.cpp \
    cdtproject.cpp \
    cdtimagelayer.cpp \
    cdtsegmentationlayer.cpp \
    cdtclassification.cpp \
    cdtprojecttabwidget.cpp \
    cdtprojectwidget.cpp \
    cdtprojecttreeitem.cpp \    
    cdtbaseobject.cpp \
    recentfilesupervisor.cpp \    
    cdtattributegenerator.cpp \
    cdtmaptoolselecttrainingsamples.cpp \
    dialognewsegmentation.cpp\    
    dialoggenerateattributes.cpp \    
    cdtattributedockwidget.cpp \
    cdtsampledockwidget.cpp \
    wizardnewclassification.cpp \
    cdtapplication.cpp \
    cdtextractionlayer.cpp \
    dialognewextraction.cpp \
    cdtextractiondockwidget.cpp \
    cdtclassificationhelper.cpp


HEADERS  += \    
    mainwindow.h \
    cdtbaseobject.h \
    cdtproject.h \
    cdtimagelayer.h \
    cdtsegmentationlayer.h \
    cdtclassification.h \
    cdtprojecttabwidget.h \
    cdtprojectwidget.h \
    cdtprojecttreeitem.h \
    cdtpluginloader.h \
    recentfilesupervisor.h \
    cdtattributegenerator.h \
    cdtbasethread.h \
    cdtmaptoolselecttrainingsamples.h \
    dialognewsegmentation.h \
    dialoggenerateattributes.h \    
    cdtattributedockwidget.h \
    cdtsampledockwidget.h \
    wizardnewclassification.h \
    cdtvariantconverter.h \
    cdtapplication.h \
    cdtextractionlayer.h \
    dialognewextraction.h \
    cdtextractiondockwidget.h \
    ../Interfaces/cdtextractioninterface.h \
    cdtclassificationhelper.h

FORMS    += \
    mainwindow.ui \
    dialognewsegmentation.ui\
    dialoggenerateattributes.ui \    
    cdtattributedockwidget.ui \
    cdtsampledockwidget.ui \
    wizardnewclassification.ui \
    dialognewextraction.ui \
    cdtextractiondockwidget.ui

INCLUDEPATH += \
    ../Interfaces \
    ../Tools/CDTDialogs \
    ../Tools/QPropertyEditor \
    ../Tools/CDTHistogramPlot \
    ../Tools/QtColorPicker\
    ../Tools/CDTFileSystem\
    ../Tools/QgsWidgets\
    ../Tools\
    ../Tools/CDTClassifierAssessmentWidget


DEPENDPATH += \
    ../Tools/CDTDialogs \
    ../Tools/QPropertyEditor \
    ../Tools/CDTHistogramPlot \
    ../Tools/QtColorPicker\
    ../Tools/CDTFileSystem\
    ../Tools/QgsWidgets\
    ../Tools/log4qt\
    ../Tools/CDTClassifierAssessmentWidget

#Libraries
unix{
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lgdal -lgomp

INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
/usr/include/qgis \
/usr/local/include/qgisS

DEFINES += CORE_EXPORT=
DEFINES += GUI_EXPORT=
}
!unix{
include(../Tools/Config/win.pri)
LIBS += -lgdal_i
}

LIBS +=     -L../lib -lCDTDialogs -lQPropertyEditor -lCDTHistogramPlot -lQtColorPicker\
             -lCDTFileSystem -lQgsWidgets -llog4qt -lCDTClassifierAssessmentWidget\
            -lstxxl -lqgis_core -lqgis_gui -lqgis_analysis -lqgis_networkanalysis -lqwt

#opencv
include(../Tools/Config/link2opencv.pri)

RESOURCES += \
    ../resource.qrc

