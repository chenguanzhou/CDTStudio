#ifndef CDTSEGMENTATIONLAYER_H
#define CDTSEGMENTATIONLAYER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>

class CDTAttributes;
class CDTClassification;
class CDTSample;

class CDTSegmentationLayer
{
public:
    explicit CDTSegmentationLayer();
private:
    QString name;
    QString shapefilePath;
    QString dbPath;
    QString method;
    QMap<QString,QVariant> params;
    QVector<CDTClassification *> classifications;
    CDTAttributes *attributes;
    QMap<QString,CDTSample*> samples;
};

#endif // CDTSEGMENTATIONLAYER_H
