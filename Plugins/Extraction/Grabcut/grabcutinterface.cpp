#include "grabcutinterface.h"
#include "cdtgrabcutmaptool.h"
#include <qgsvectorlayer.h>

GrabcutInterface::GrabcutInterface(QObject *parent)
:CDTExtractionInterface(parent)
{

}

QString GrabcutInterface::methodName() const
{
    return tr("Grab cut");
}

QString GrabcutInterface::description() const
{
    return tr("Grab cut active contour");
}

QgsMapTool *GrabcutInterface::mapTool(QgsMapCanvas *canvas, QString imagePath, QgsVectorLayer *vectorLayer)
{
    CDTGrabcutMapTool *grabcutMapTool = new CDTGrabcutMapTool(canvas);
    grabcutMapTool->imagePath = imagePath;
    grabcutMapTool->vectorLayer = vectorLayer;
    return grabcutMapTool;
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Grab cut, GrabcutInterface)
#endif
