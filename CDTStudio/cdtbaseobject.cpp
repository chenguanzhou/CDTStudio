#include "cdtbaseobject.h"
#include <QtCore>
#include "cdtprojecttreeitem.h"
#include <qgsmaplayer.h>

CDTBaseObject::CDTBaseObject(QObject *parent) :
    QObject(parent),
    mapCanvasLayer(NULL)
{
    if (parent) connect(this,SIGNAL(appendLayer(QList<QgsMapLayer*> )),parent,SIGNAL(appendLayer(QList<QgsMapLayer*>)));
    if (parent) connect(this,SIGNAL(removeLayer(QList<QgsMapLayer*> )),parent,SIGNAL(removeLayer(QList<QgsMapLayer*>)));
}

QList<QStandardItem *> CDTBaseObject::standardItems() const
{
    return QList<QStandardItem *>()<<(QStandardItem *)keyItem<<(QStandardItem *)valueItem;
}
