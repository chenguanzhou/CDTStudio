#ifndef GRABCUTINTERFACE_H
#define GRABCUTINTERFACE_H

#include "cdtextractioninterface.h"

class PartialSegmentationInterface : public CDTExtractionInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTExtractionInterface" FILE "PartialSegmentation.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(CDTExtractionInterface)
public:
    PartialSegmentationInterface(QObject *parent = 0);

    QString methodName()    const;
    QString description()   const;

    QgsMapTool *mapTool(QgsMapCanvas* canvas,QString imagePath,QgsVectorLayer *vectorLayer);
};

#endif // GRABCUTINTERFACE_H
