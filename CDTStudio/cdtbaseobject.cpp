#include "cdtbaseobject.h"
#include <QtCore>
#include "cdtprojecttreeitem.h"
#include <qgsmaplayer.h>
#include <qgsmapcanvas.h>

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
//    delete mapCanvasLayer;
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

void CDTBaseObject::setMapCanvas(QgsMapCanvas *canvas)
{
    mapCanvas = canvas;
}
