#include "cdtbaseobject.h"
#include <QtCore>
#include "cdtprojecttreeitem.h"

CDTBaseObject::CDTBaseObject(QObject *parent) :
    QObject(parent)
{
}

QList<QStandardItem *> CDTBaseObject::standardItems() const
{
    return QList<QStandardItem *>()<<(QStandardItem *)keyItem<<(QStandardItem *)valueItem;
}
