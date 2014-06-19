#include "cdtbaseobject.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "cdtproject.h"

CDTBaseObject::CDTBaseObject(QUuid uuid, QObject *parent) :
    QObject(parent),
    mapCanvasLayer(NULL),
    mapCanvas(NULL)
{
    this->uuid = uuid;
    if (parent) connect(this,SIGNAL(appendLayers(QList<QgsMapLayer*> )),parent,SIGNAL(appendLayers(QList<QgsMapLayer*>)));
    if (parent) connect(this,SIGNAL(removeLayer(QList<QgsMapLayer*> )),parent,SIGNAL(removeLayer(QList<QgsMapLayer*>)));
    if (parent) mapCanvas = ((CDTBaseObject*)parent)->mapCanvas;
}

CDTBaseObject::~CDTBaseObject()
{

}

QList<QStandardItem *> CDTBaseObject::standardItems() const
{
    return QList<QStandardItem *>()<<(QStandardItem *)keyItem<<(QStandardItem *)valueItem;
}

QgsMapLayer *CDTBaseObject::canvasLayer() const
{
    return mapCanvasLayer;
}

QgsMapCanvas *CDTBaseObject::canvas() const
{
    return mapCanvas;
}

CDTProject *CDTBaseObject::rootProject() const
{
    QObject *obj = (QObject *)this;
    while (obj->parent())
        obj = obj->parent();
    return (CDTProject *)obj;
}

CDTFileSystem *CDTBaseObject::fileSystem() const
{
    return rootProject()->fileSystem;
}

void CDTBaseObject::setMapCanvas(QgsMapCanvas *canvas)
{
    mapCanvas = canvas;
}
