#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T15:33:54
#
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS += \
    Tools\
    Plugins \
    CDTStudio \
    Tests

TRANSLATIONS  += Trans/zh_CN.ts

Tests.depends = CDTStudio
CDTStudio.depends = Tools


