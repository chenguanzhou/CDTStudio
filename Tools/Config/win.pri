OSGEO4W_ROOT = $(OSGEO4W_ROOT)
QGIS_ROOT = $(QGIS_ROOT)
STXXL_ROOT = $(STXXL_ROOT)
QWT_ROOT = $(QWT_ROOT)
BOOST_ROOT = $(BOOST_ROOT)
BOOST_LIBRARYDIR = $(BOOST_LIBRARYDIR)

greaterThan(QT_MAJOR_VERSION, 4){
    GDAL_ROOT = $(GDAL_ROOT)
    !isEmpty(GDAL_ROOT) {
        INCLUDEPATH += $(GDAL_ROOT)/include
        LIBS += -L$(GDAL_ROOT)/lib
    }
    GEOS_INCLUDE_PATH = $(GEOS_INCLUDE_PATH)
    !isEmpty(GEOS_INCLUDE_PATH) {
        INCLUDEPATH += $(GEOS_INCLUDE_PATH)
    }
}
else{
    !isEmpty(OSGEO4W_ROOT) {
        INCLUDEPATH += $(OSGEO4W_ROOT)/include
        LIBS += -L$(OSGEO4W_ROOT)/lib
    }
    !isEmpty(BOOST_ROOT) {
        INCLUDEPATH += $(BOOST_ROOT)
    }

    !isEmpty(BOOST_LIBRARYDIR) {
        LIBS += -L$(BOOST_LIBRARYDIR)
    }
}

!isEmpty(QGIS_ROOT) {
    INCLUDEPATH += $(QGIS_ROOT)\include
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


DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)

# disable warnings
greaterThan(QT_MAJOR_VERSION, 4){
DEFINES += _XKEYCHECK_H
DEFINES += _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
} else {
DEFINES += noexcept=
}
