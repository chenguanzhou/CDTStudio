#ifndef ERTREESINTERFACE_H
#define ERTREESINTERFACE_H

#include "cdtclassifierinterface.h"

class ERTreesInterface : public CDTClassifierInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTClassifierInterface" FILE "ERTrees.json")
#else
    Q_INTERFACES(CDTClassifierInterface)
#endif // QT_VERSION >= 0x050000

public:    
    ERTreesInterface(QObject *parent = 0);
    QString classifierName() const;

    cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses);
};

#endif // ERTREESINTERFACE_H
