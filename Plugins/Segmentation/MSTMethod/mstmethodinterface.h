#ifndef MSTMETHODINTERFACE_H
#define MSTMETHODINTERFACE_H

//#include "../../../Interfaces/cdtsegmentationInterface.h"
#include "cdtsegmentationInterface.h"
class FormMST;
class MSTMethodInterface:public CDTSegmentationInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTSegmentationInterface" FILE "MSTMethod.json")
#else
    Q_INTERFACES(CDTSegmentationInterface)
#endif // QT_VERSION >= 0x050000
public:
    explicit MSTMethodInterface(QObject* parent = 0);
    ~MSTMethodInterface();
    QString segmentationMethod()const;
    QWidget *paramsForm() ;

private:


};

#endif // MSTMETHODINTERFACE_H
