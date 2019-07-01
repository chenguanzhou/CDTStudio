#include "PartialSegmentationInterface.h"
#include "CdtPartialSegmentationMapTool.h"
#include <qgsvectorlayer.h>

PartialSegmentationInterface::PartialSegmentationInterface(QObject *parent)
:CDTExtractionInterface(parent)
{

}

QString PartialSegmentationInterface::methodName() const
{
    return tr("Partial Segmentation");
}

QString PartialSegmentationInterface::description() const
{
    return tr("Partial Segmentation");
}

QgsMapTool *PartialSegmentationInterface::mapTool(QgsMapCanvas *canvas, QString imagePath, QgsVectorLayer *vectorLayer)
{
    CDTPartialSegmentationMapTool *partialSegmentMapTool = new CDTPartialSegmentationMapTool(canvas);
    partialSegmentMapTool->imagePath = imagePath;
    partialSegmentMapTool->vectorLayer = vectorLayer;
    return partialSegmentMapTool;
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Grab cut, GrabcutInterface)
#endif
