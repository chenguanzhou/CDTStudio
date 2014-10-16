#ifndef CDTTASK_PBCDBINARY_H
#define CDTTASK_PBCDBINARY_H

#include "cdttask.h"
#include <QPair>

class GDALDataset;
class GDALDriver;

class CDTPBCDDiffInterface;
class CDTPBCDMergeInterface;
class CDTAutoThresholdInterface;

class CDTTask_PBCDBinary : public CDTTask
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit CDTTask_PBCDBinary(QString id, QDomDocument params, QObject *parent = 0);

signals:

public slots:
    void start();
private:
    bool validateParams();
    QString readParams();
    GDALDataset *generateAveImage(GDALDriver *poDriver, GDALDataset *poSrcDS);
    void merge(GDALDataset *poDS, GDALDataset *poMerged);

    GDALDataset* poT1DS;
    GDALDataset* poT2DS;
    QList<QPair<uint,uint> > bandPairs;
    QString radiometricCorrectionMethod;
    QString diffMethod;
    QString mergeMethod;
    QString autoThreshold;
    CDTPBCDDiffInterface *diffPlugin;
    CDTPBCDMergeInterface *mergePlugin;
    CDTAutoThresholdInterface *autoThresholdPlugin;
    double positiveThreshold,negetiveThreshold;
    bool isDoubleThreshold;    
};

#endif // CDTTASK_PBCDBINARY_H
