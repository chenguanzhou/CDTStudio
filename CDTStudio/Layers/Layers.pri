HEADERS += \
    Layers/cdtbaselayer.h \
    Layers/cdtextractionlayer.h \
    Layers/cdtimagelayer.h \
    Layers/cdtsegmentationlayer.h \
    Layers/cdtclassificationlayer.h \
    Layers/cdtprojectlayer.h \
    $$PWD/cdtpbcdbinarylayer.h \
    $$PWD/cdtchangelayer.h

SOURCES += \
    Layers/cdtbaselayer.cpp \
    Layers/cdtextractionlayer.cpp \
    Layers/cdtimagelayer.cpp \
    Layers/cdtsegmentationlayer.cpp \
    Layers/cdtclassificationlayer.cpp \
    Layers/cdtprojectlayer.cpp \
    $$PWD/cdtpbcdbinarylayer.cpp \
    $$PWD/cdtchangelayer.cpp

INCLUDEPATH += $$PWD
