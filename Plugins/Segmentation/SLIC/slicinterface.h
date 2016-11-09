#ifndef MSTMETHODINTERFACE_H
#define MSTMETHODINTERFACE_H

#include <map>
#include <vector>
#include <limits>
#include <QObject>
#include <QMap>
#include <QVector>
#include "cdtsegmentationinterface.h"


class SLICInterface:public CDTSegmentationInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTSegmentationInterface" FILE "SLIC.json")
#else
    Q_INTERFACES(CDTSegmentationInterface)
#endif // QT_VERSION >= 0x050000

public:
    explicit SLICInterface(QObject* parent = 0);
    ~SLICInterface();
    QString segmentationMethod()const;
    void startSegmentation();

private:
};

#endif // MSTMETHODINTERFACE_H
