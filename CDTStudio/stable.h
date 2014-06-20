#ifndef STABLE_H
#define STABLE_H

//Qt Component
#include <QtCore>
#include <QtGui>
#include <QtXml>
#include <QtSql>
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
#include <qgisgui.h>
#include <qgsrasterlayer.h>
#include <qgsvectorlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgscategorizedsymbolrendererv2.h>
#include <qgsrendererv2widget.h>
#include <qgsfillsymbollayerv2.h>
#include <qgsvectordataprovider.h>
#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsmaptoolzoom.h>
#include <qgsmaptoolpan.h>
#include "qgsrubberband.h"

//Log4Qt
#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"

#endif // STABLE_H
