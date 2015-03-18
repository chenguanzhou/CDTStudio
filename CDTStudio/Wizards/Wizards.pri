FORMS += \
    $$PWD/wizardnewclassification.ui \
    $$PWD/wizardvectorchangedetection.ui

HEADERS += \
    $$PWD/wizardnewclassification.h \
    $$PWD/wizardvectorchangedetection.h\
    $$PWD/Helpers/cdtclassificationhelper.h\
    $$PWD/Helpers/cdtvectorchangedetectionhelper.h

SOURCES += \
    $$PWD/wizardnewclassification.cpp \
    $$PWD/wizardvectorchangedetection.cpp\
    $$PWD/Helpers/cdtclassificationhelper.cpp\
    $$PWD/Helpers/cdtvectorchangedetectionhelper.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/Helpers
