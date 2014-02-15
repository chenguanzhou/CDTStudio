#ifndef CDTSEGMENTATIONLAYER_H
#define CDTSEGMENTATIONLAYER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>
#include "cdtattributes.h"

class CDTClassification;
class CDTSample;

class CDTSegmentationLayer
{
public:
    explicit CDTSegmentationLayer();
private:
    QString name;
    QString shapefilePath;    
    QString method;
    QMap<QString,QVariant> params;
    QVector<CDTClassification *> classifications;
    CDTAttributes attributes;
    QMap<QString,CDTSample*> samples;
};

#endif // CDTSEGMENTATIONLAYER_H
