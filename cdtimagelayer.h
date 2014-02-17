#ifndef CDTIMAGELAYER_H
#define CDTIMAGELAYER_H

#include <QObject>
#include <QVector>
#include <QtCore>
#include "cdtsegmentationlayer.h"
#include "cdtclassification.h"
#include "cdtprojecttreeitem.h"

class CDTImageLayer
{
public:
    explicit CDTImageLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
    friend QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);

    void updateTreeModel(CDTProjectTreeItem* parent);

private:
    QString path;
    QVector<CDTSegmentationLayer> segmentations;


};

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
#endif // CDTIMAGELAYER_H
