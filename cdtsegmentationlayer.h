#ifndef CDTSEGMENTATIONLAYER_H
#define CDTSEGMENTATIONLAYER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVariant>
#include "cdtattributes.h"
#include "cdtsample.h"


class CDTClassification;
class CDTSegmentationLayer
{

public:
    explicit CDTSegmentationLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
    friend QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);

//    QString                      name()const {return _name;}
//    QString                      shapefilePath()const {return _shapefilePath;}
//    QString                      method()const {return _method;}
//    QMap<QString,QVariant>       params()const {return _params;}
//    QVector<CDTClassification >  classifications()const {return _classifications;}
//    CDTAttributes                attributes()const {return _attributes;}
//    QMap<QString,CDTSample>      samples()const{return _samples;}
private:
    QString name;
    QString shapefilePath;
    QString method;
    QMap<QString,QVariant> params;
    QVector<CDTClassification> classifications;
    CDTAttributes attributes;
    QMap<QString,CDTSample> samples;
};
QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);
#endif // CDTSEGMENTATIONLAYER_H
