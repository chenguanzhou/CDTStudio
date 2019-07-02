#ifndef MSTMETHODINTERFACE_H
#define MSTMETHODINTERFACE_H

#include <map>
#include <vector>
#include <limits>
#include <QObject>
#include <QMap>
#include <QVector>
#include "cdtsegmentationinterface.h"


class SLICMethodPrivate;

class SLICInterface:public CDTSegmentationInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTSegmentationInterface" FILE "SLIC.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(CDTSegmentationInterface)
    Q_PROPERTY(double Object_Count READ objectCount WRITE setObjectCount DESIGNABLE true USER true)
    Q_PROPERTY(int Compactness READ compactness WRITE setCompactness DESIGNABLE true USER true)
    Q_PROPERTY(Algorithm algorith READ algorith WRITE setAlgorith DESIGNABLE true USER true)

    Q_ENUMS(Algorithm)

public:
    enum Algorithm {SLIC = 100 , SLICO = 101};

    explicit SLICInterface(QObject* parent = 0);
    ~SLICInterface();
    QString segmentationMethod()const;
    void startSegmentation();

    double  objectCount()const;
    int compactness() const;
    Algorithm algorith() const;

    void setObjectCount(double val);
    void setCompactness(int val);
    void setAlgorith(Algorithm val);

private:
    SLICMethodPrivate *pData;
};

#endif // MSTMETHODINTERFACE_H
