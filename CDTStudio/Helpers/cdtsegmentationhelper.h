#ifndef CDTSEGMENTATIONHELPER_H
#define CDTSEGMENTATIONHELPER_H

#include "cdtbasethread.h"

class CDTSegmentationInterface;
class CDTSegmentationHelper : public CDTBaseThread
{
public:
    explicit CDTSegmentationHelper(CDTSegmentationInterface *plugin, QObject *parent = NULL);
    ~CDTSegmentationHelper();

    bool isValid()const;

    void setMarkfilePath  (const QString &path);
    void setShapefilePath (const QString &path);

public:
    void run();

protected:
    CDTSegmentationInterface *plugin;
    QString markfilePath;
    QString shapefilePath;
};

#endif // CDTSEGMENTATIONHELPER_H
