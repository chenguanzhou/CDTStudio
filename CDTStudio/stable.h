#ifndef STABLE_H
#define STABLE_H

//Qt Component
#include <QtCore>
#include <QtGui>
#include <QtXml>
#include <QtSql>
#include <QtNetwork>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

//GDAL
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>

//OpenCV
#include <opencv2/opencv.hpp>

//QGIS
#include <qgis.h>
#include <qgsapplication.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
//#include <qgsmaplayerregistry.h>
#include <qgssinglesymbolrenderer.h>
#include <qgscategorizedsymbolrenderer.h>
#include <qgsfillsymbollayer.h>
#include <qgsvectordataprovider.h>
#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsmaptoolzoom.h>
#include <qgsmaptoolpan.h>
#include <qgsrubberband.h>
#include <qgspolygon.h>
#include <qgslinestring.h>
#include <qgsmapsettings.h>
#include <qgsmapmouseevent.h>

#endif // STABLE_H
