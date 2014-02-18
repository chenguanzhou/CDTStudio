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
    explicit CDTImageLayer(const QString &p,const QString &n);

    friend QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
    friend QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);

    void updateTreeModel(CDTProjectTreeItem* parent);


private:
    QString path;
    QString name;
    QVector<CDTSegmentationLayer> segmentations;
    void setsegmentations(QVector<CDTSegmentationLayer> &s);

};

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
#endif // CDTIMAGELAYER_H
