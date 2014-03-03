#ifndef MSTMETHODINTERFACE_H
#define MSTMETHODINTERFACE_H

#include "../../../Interfaces/cdtsegmentationInterface.h"
class FormMST;
class MSTMethodInterface:public CDTSegmentationInterface
{
    Q_OBJECT
    Q_INTERFACES(CDTSegmentationInterface)
public:
    explicit MSTMethodInterface(QObject* parent = 0);
    ~MSTMethodInterface();
    QString segmentationMethod()const;
    QWidget *paramsForm() ;

private:


};

#endif // MSTMETHODINTERFACE_H
