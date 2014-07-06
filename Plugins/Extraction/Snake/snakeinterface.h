#ifndef SNAKEINTERFACE_H
#define SNAKEINTERFACE_H

#include "cdtextractioninterface.h"

class SnakeInterface : public CDTExtractionInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTExtractionInterface" FILE "Snake.json")
#else
    Q_INTERFACES(CDTExtractionInterface)
#endif // QT_VERSION >= 0x050000
public:
    SnakeInterface(QObject *parent = 0);

    QString methodName()    const;
    QString description()   const;

    QgsMapTool *mapTool(QgsMapCanvas* canvas,QString imagePath,QgsVectorLayer *vectorLayer);
};

#endif // SNAKEINTERFACE_H
