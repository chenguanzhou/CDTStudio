#ifndef CDTIMAGELAYER_H
#define CDTIMAGELAYER_H

#include <QObject>
#include <QVector>

class CDTSegmentationLayer;
class CDTImageLayer
{
public:
    explicit CDTImageLayer();

private:
    QString path;
    QVector<CDTSegmentationLayer *> segmentations;
};

#endif // CDTIMAGELAYER_H
