#ifndef CDTPBCDMERGEINTERFACE_H
#define CDTPBCDMERGEINTERFACE_H

#include <QtCore>
#include <QtPlugin>

class GDALRasterBand;
class GDALDataset;
class CDTPBCDMergeInterface:public QObject
{
    Q_OBJECT
public:
    explicit CDTPBCDMergeInterface(QObject* parent = 0):QObject(parent){}

    virtual QString methodName()const =0;

    virtual float merge(QVector<float> buffer) =0;
};

Q_DECLARE_INTERFACE(CDTPBCDMergeInterface,"cn.edu.WHU.CDTStudio.CDTPBCDMergeInterface/1.0")

#endif // CDTPBCDMERGEINTERFACE_H
