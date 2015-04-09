#ifndef AutoSVMINTERFACE_H
#define AutoSVMINTERFACE_H

#include "cdtclassifierinterface.h"

class AutoSVMInterface : public CDTClassifierInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTClassifierInterface" FILE "AutoSVM.json")
#else
    Q_INTERFACES(CDTClassifierInterface)
#endif // QT_VERSION >= 0x050000

public:    
    AutoSVMInterface(QObject *parent = 0);
    ~AutoSVMInterface();
    QString classifierName() const;

    cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses);
};

#endif // AutoSVMINTERFACE_H
