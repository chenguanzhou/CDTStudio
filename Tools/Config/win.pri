OSGEO4W_ROOT = $(OSGEO4W_ROOT)
QGIS_ROOT = $(QGIS_ROOT)
STXXL_ROOT = $(STXXL_ROOT)
QWT_ROOT = $(QWT_ROOT)
BOOST_ROOT = $(BOOST_ROOT)
BOOST_LIBRARYDIR = $(BOOST_LIBRARYDIR)

!isEmpty(OSGEO4W_ROOT) {
    INCLUDEPATH += $(OSGEO4W_ROOT)/include
    LIBS += -L$(OSGEO4W_ROOT)/lib
}

!isEmpty(QGIS_ROOT) {
    INCLUDEPATH += $(QGIS_ROOT)/include
    LIBS += -L$(QGIS_ROOT)/lib
}

!isEmpty(STXXL_ROOT) {
    INCLUDEPATH += $(STXXL_ROOT)/include
    LIBS += -L$(STXXL_ROOT)/lib
}

!isEmpty(QWT_ROOT) {
    INCLUDEPATH += $(QWT_ROOT)/include
    LIBS += -L$(QWT_ROOT)/lib
    include($(QWT_ROOT)/features/qwt.prf)
}

!isEmpty(BOOST_ROOT) {
    INCLUDEPATH += $(BOOST_ROOT)
}

!isEmpty(BOOST_LIBRARYDIR) {
    LIBS += -L$(BOOST_LIBRARYDIR)
}

DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)
