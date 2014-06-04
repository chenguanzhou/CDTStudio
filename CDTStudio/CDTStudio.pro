#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql xml svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDTStudio
TEMPLATE = app
CONFIG+=precompile_header qwt
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
    cdtattributedockwidget.cpp \
    cdtsampledockwidget.cpp \
    wizardnewclassification.cpp


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
    cdtattributegenerator.h \
    cdtbasethread.h \
    cdtmaptoolselecttrainingsamples.h \
    dialognewsegmentation.h \
    dialoggenerateattributes.h \    
    cdtattributedockwidget.h \
    cdtsampledockwidget.h \
    wizardnewclassification.h \
    cdtvariantconverter.h


FORMS    += mainwindow.ui \            
    dialognewsegmentation.ui\
    dialoggenerateattributes.ui \    
    cdtattributedockwidget.ui \
    cdtsampledockwidget.ui \
    wizardnewclassification.ui

INCLUDEPATH += ../Interfaces \
../Tools/CDTDialogs \
../Tools/QPropertyEditor \
../Tools/CDTHistogramPlot \
../Tools/QtColorPicker


DEPENDPATH += \
../Tools/CDTDialogs \
../Tools/QPropertyEditor \
../Tools/CDTHistogramPlot \
../Tools/QtColorPicker

unix{
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lgdal -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_imgproc -lgomp

INCLUDEPATH += /usr/include/gdal \
/usr/local/include/gdal \
INCLUDEPATH += /usr/include/qgis \
/usr/local/include/qgis \

DEFINES += CORE_EXPORT=
DEFINES += GUI_EXPORT=
}
!unix{
include(../Tools/Config/win.pri)
LIBS += -lgdal_i
}

LIBS += -L../lib -lCDTDialogs -lQPropertyEditor -lCDTHistogramPlot -lQtColorPicker\
 -lstxxl -lqgis_core -lqgis_gui -lqgis_analysis -lqgis_networkanalysis -lqwt

RESOURCES += \
    ../resource.qrc

