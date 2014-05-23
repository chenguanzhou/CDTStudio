INCLUDEPATH += $(OSGEO4W_ROOT)/include $(QGIS_ROOT)/include $(STXXL_ROOT)/include $(QWT_ROOT)/include $(BOOST_ROOT)
LIBS += -L$(OSGEO4W_ROOT)/lib -L$(QGIS_ROOT)/lib -L$(STXXL_ROOT)/lib -L$(QWT_ROOT)/lib -L$(BOOST_LIBRARYDIR)

DEFINES += CORE_EXPORT=__declspec(dllimport)
DEFINES += GUI_EXPORT=__declspec(dllimport)
