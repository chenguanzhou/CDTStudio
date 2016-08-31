#ifndef GRABCUTINTERFACE_H
#define GRABCUTINTERFACE_H

#include "cdtextractioninterface.h"

class GrabcutInterface : public CDTExtractionInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTExtractionInterface" FILE "Grabcut.json")
#else
    Q_INTERFACES(CDTExtractionInterface)
#endif // QT_VERSION >= 0x050000
public:
    GrabcutInterface(QObject *parent = 0);

    QString methodName()    const;
    QString description()   const;

    QgsMapTool *mapTool(QgsMapCanvas* canvas,QString imagePath,QgsVectorLayer *vectorLayer);
};

#endif // GRABCUTINTERFACE_H
