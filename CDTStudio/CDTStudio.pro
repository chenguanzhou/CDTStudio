#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

QT       += core gui sql xml svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.2
TARGET = CDTStudio
TEMPLATE = app
DESTDIR = ../bin

#PRECOMPILED HEADER
CONFIG+=precompile_header qwt
PRECOMPILED_HEADER = stable.h

SOURCES += main.cpp\
    mainwindow.cpp \
    cdtprojecttabwidget.cpp \
    cdtprojectwidget.cpp \
    cdtprojecttreeitem.cpp \
    recentfilesupervisor.cpp \    
    cdtattributegenerator.cpp \
    cdtapplication.cpp    

HEADERS  += \    
    mainwindow.h \
    cdtprojecttabwidget.h \
    cdtprojectwidget.h \
    cdtprojecttreeitem.h \
    cdtpluginloader.h \
    recentfilesupervisor.h \
    cdtattributegenerator.h \
    cdtbasethread.h \
    cdtvariantconverter.h \
    cdtapplication.h 
	
FORMS    += \
    mainwindow.ui \

include(Dialogs/Dialogs.pri)
include(Docks/Docks.pri)
include(Helpers/Helpers.pri)
include(Layers/Layers.pri)
include(Wizards/Wizards.pri)
include(../Interfaces/Interfaces.pri)

INCLUDEPATH += \
    ../Interfaces \
    ../Tools/QPropertyEditor \
    ../Tools/CDTHistogramPlot \
    ../Tools/QtColorPicker\
    ../Tools/CDTFileSystem\
    ../Tools\
    ../Tools/CDTClassifierAssessmentWidget\
    ../Tools/CDTTableExporter


DEPENDPATH += \
    ../Tools/QPropertyEditor \
    ../Tools/CDTHistogramPlot \
    ../Tools/QtColorPicker\
    ../Tools/CDTFileSystem\
    ../Tools/log4qt\
    ../Tools/CDTClassifierAssessmentWidget\
    ../Tools/CDTTableExporter

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
LIBS += -lgdal_i -lgdi32
}

LIBS +=     -L../lib -lQPropertyEditor -lCDTHistogramPlot -lQtColorPicker\
             -lCDTFileSystem -llog4qt -lCDTTableExporter\
            -lstxxl -lqgis_core -lqgis_gui -lqgis_analysis -lqgis_networkanalysis -lqwt

#opencv
include(../Tools/Config/link2opencv.pri)

RESOURCES += \
    ../resource.qrc
