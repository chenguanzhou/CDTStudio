#ifndef CDTPBCDDIFF_H
#define CDTPBCDDIFF_H

#include "cdtbasethread.h"
#include <QPair>

class CDTPBCDDiffInterface;
class CDTPBCDMergeInterface;
class GDALDataset;
class GDALRasterBand;

class CDTPBCDDiff : public CDTBaseThread
{
    Q_OBJECT
public:
    explicit CDTPBCDDiff(
            const QString &T1Path,
            const QString &T2Path,
            QList<QPair<uint,uint> > BandPairs,
            CDTPBCDDiffInterface *DiffPlugin,
            CDTPBCDMergeInterface *MergePlugin,
            QObject *parent = 0);
    ~CDTPBCDDiff();

    void run();
    bool isCompleted()const;
    QString outputPath;
    int numOfThresholds;

private:
    QString t1Path;
    QString t2Path;
    QList<QPair<uint,uint> > bandPairs;
    CDTPBCDDiffInterface *diffPlugin;
    CDTPBCDMergeInterface *mergePlugin;

    bool completed;
};

class CDTPBCDGenerateResult:public CDTBaseThread
{
    Q_OBJECT
public:
    explicit CDTPBCDGenerateResult(GDALDataset* mergeDS,GDALDataset* resultDS,int numOfThresholds,double posT,double negT, QObject *parent = NULL);
    ~CDTPBCDGenerateResult();
    void run();
private:
    GDALDataset* poMergeDS;
    GDALDataset* poResultDS;

    int numOfThresholds;

    double posT;
    double negT;
};

class CDTPBCDHistogramHelper:public CDTBaseThread
{
    Q_OBJECT
public:
    friend class WizardPagePBCDAutoThreshold;
    explicit CDTPBCDHistogramHelper(GDALDataset *ds,int numOfThresholds,QObject *parent = NULL);

    void run();
private:
    GDALDataset *poDS;
    int numOfThresholds;
    QVector<GUIntBig> histogramPositive;
    QVector<GUIntBig> histogramNegetive;
    double maxVal;
    double minVal;
};

#endif // CDTPBCDDIFF_H
