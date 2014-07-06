#include "snakeinterface.h"
#include "cdtsnakemaptool.h"
#include <qgsvectorlayer.h>

SnakeInterface::SnakeInterface(QObject *parent)
    :CDTExtractionInterface(parent)
{
}

QString SnakeInterface::methodName() const
{
    return tr("Snake");
}

QString SnakeInterface::description() const
{
    return tr("Snake active contour");
}

QgsMapTool *SnakeInterface::mapTool(QgsMapCanvas *canvas,QString imagePath,QgsVectorLayer *vectorLayer)
{
    CDTSnakeMapTool *snakeMapTool = new CDTSnakeMapTool(canvas);
    snakeMapTool->imagePath = imagePath;
    snakeMapTool->vectorLayer = vectorLayer;
    return snakeMapTool;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Snake, SnakeInterface)
#endif // QT_VERSION < 0x050000
