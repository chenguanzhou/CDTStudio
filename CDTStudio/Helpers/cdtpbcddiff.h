#ifndef CDTPBCDDIFF_H
#define CDTPBCDDIFF_H

#include "cdtbasethread.h"
#include <QPair>

class CDTPBCDDiffInterface;
class CDTPBCDMergeInterface;

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

#endif // CDTPBCDDIFF_H
