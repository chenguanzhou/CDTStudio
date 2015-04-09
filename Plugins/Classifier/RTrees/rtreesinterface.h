#ifndef RTREESINTERFACE_H
#define RTREESINTERFACE_H

#include "cdtclassifierinterface.h"

class RTreesInterface : public CDTClassifierInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTClassifierInterface" FILE "RTrees.json")
#else
    Q_INTERFACES(CDTClassifierInterface)
#endif // QT_VERSION >= 0x050000

public:    
    RTreesInterface(QObject *parent = 0);
    QString classifierName() const;

    cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses);
};

#endif // RTREESINTERFACE_H
