#ifndef CDTAUTOTHRESHOLDINTERFACE_H
#define CDTAUTOTHRESHOLDINTERFACE_H

#include <QtCore>
#include <QtPlugin>

class GDALRasterBand;
class GDALDataset;
class CDTAutoThresholdInterface:public QObject
{
    Q_OBJECT
public:
    explicit CDTAutoThresholdInterface(QObject* parent = 0):QObject(parent){}

    virtual QString methodName()const =0;

    virtual float autoThreshold(QVector<int> histogram) =0;
};

Q_DECLARE_INTERFACE(CDTAutoThresholdInterface,"cn.edu.WHU.CDTStudio.CDTAutoThresholdInterface/1.0")

#endif // CDTAUTOTHRESHOLDINTERFACE_H
