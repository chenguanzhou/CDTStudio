#include "cdtbaselayer.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "cdtprojectlayer.h"

CDTBaseLayer::CDTBaseLayer(QUuid uuid, QObject *parent) :
    QObject(parent),
    mapCanvasLayer(NULL),
    mapCanvas(NULL)
{
    this->uuid = uuid;
    if (parent) connect(this,SIGNAL(appendLayers(QList<QgsMapLayer*> )),parent,SIGNAL(appendLayers(QList<QgsMapLayer*>)));
    if (parent) connect(this,SIGNAL(removeLayer(QList<QgsMapLayer*> )),parent,SIGNAL(removeLayer(QList<QgsMapLayer*>)));
    if (parent) mapCanvas = ((CDTBaseLayer*)parent)->mapCanvas;
}

CDTBaseLayer::~CDTBaseLayer()
{

}

QList<QStandardItem *> CDTBaseLayer::standardItems() const
{
    return QList<QStandardItem *>()<<(QStandardItem *)keyItem<<(QStandardItem *)valueItem;
}

QgsMapLayer *CDTBaseLayer::canvasLayer() const
{
    return mapCanvasLayer;
}

QgsMapCanvas *CDTBaseLayer::canvas() const
{
    return mapCanvas;
}

CDTProjectLayer *CDTBaseLayer::rootProject() const
{
    QObject *obj = (QObject *)this;
    while (obj->parent())
        obj = obj->parent();
    return (CDTProjectLayer *)obj;
}

CDTFileSystem *CDTBaseLayer::fileSystem() const
{
    return rootProject()->fileSystem;
}

void CDTBaseLayer::setMapCanvas(QgsMapCanvas *canvas)
{
    mapCanvas = canvas;
}
