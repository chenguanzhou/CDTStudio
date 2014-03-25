#include "cdtbaseobject.h"
#include <QtCore>
#include "cdtprojecttreeitem.h"
#include <qgsmaplayer.h>

CDTBaseObject::CDTBaseObject(QObject *parent) :
    QObject(parent),
    mapCanvasLayer(NULL)
{
    if (parent) connect(this,SIGNAL(appendLayers(QList<QgsMapLayer*> )),parent,SIGNAL(appendLayers(QList<QgsMapLayer*>)));
    if (parent) connect(this,SIGNAL(removeLayer(QList<QgsMapLayer*> )),parent,SIGNAL(removeLayer(QList<QgsMapLayer*>)));
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
