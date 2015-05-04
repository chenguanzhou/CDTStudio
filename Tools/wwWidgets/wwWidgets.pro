TEMPLATE = lib
TARGET = wwWidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ../../lib
DLLDESTDIR = ../../bin
DEFINES += WW_BUILD_WWWIDGETS

HEADERS += \
    QwwButtonLineEdit \
    qwwbuttonlineedit.h \
    qwwbuttonlineedit_p.h \
    QwwFileChooser \
    qwwfilechooser.h \
    wwglobal.h \
    wwglobal_p.h

SOURCES += \
    qwwbuttonlineedit.cpp \
    qwwfilechooser.cpp \
    wwglobal_p.cpp

RESOURCES += \
    images/wwwidgets.qrc
