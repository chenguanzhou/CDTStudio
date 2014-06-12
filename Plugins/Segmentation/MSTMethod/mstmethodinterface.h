#ifndef MSTMETHODINTERFACE_H
#define MSTMETHODINTERFACE_H

//#include "../../../Interfaces/cdtsegmentationInterface.h"
#include "cdtsegmentationInterface.h"
class MSTMethodPrivate;
class MSTMethodInterface:public CDTSegmentationInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTSegmentationInterface" FILE "MSTMethod.json")
#else
    Q_INTERFACES(CDTSegmentationInterface)
#endif // QT_VERSION >= 0x050000

    Q_PROPERTY(double threshold READ threshold WRITE setThreshold DESIGNABLE true USER true)
    Q_PROPERTY(int minObjectCount READ minObjectCount WRITE setMinObjectCount DESIGNABLE true USER true)
    Q_PROPERTY(bool shieldNulValue READ shieldNulValue WRITE setShieldNulValue DESIGNABLE true USER true)

public:
    explicit MSTMethodInterface(QObject* parent = 0);
    ~MSTMethodInterface();
    QString segmentationMethod()const;
    void startSegmentation(QProgressBar *progressBar,QLabel *label);

    double  threshold()const;
    int minObjectCount() const;
    bool shieldNulValue() const;

    void setThreshold(double val);
    void setMinObjectCount(int val);
    void setShieldNulValue(bool val);

private:
    MSTMethodPrivate *pData;
};

#endif // MSTMETHODINTERFACE_H
