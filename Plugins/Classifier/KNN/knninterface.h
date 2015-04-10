#ifndef KNNINTERFACE_H
#define KNNINTERFACE_H

#include "cdtclassifierinterface.h"

class KNNInterfacePrivate;
class KNNInterface : public CDTClassifierInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTClassifierInterface" FILE "KNN.json")
#else
    Q_INTERFACES(CDTClassifierInterface)
#endif // QT_VERSION >= 0x050000

    Q_PROPERTY(int maxK READ maxK WRITE setMaxK DESIGNABLE true USER true)
    Q_CLASSINFO("maxK", "minimum=1;")
public:
    KNNInterface(QObject *parent = 0);
    ~KNNInterface();
    QString classifierName() const;

    cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses);

    int maxK()const;

public slots:
    void setMaxK(int k);

private:
    KNNInterfacePrivate* data;
};

#endif // KNNINTERFACE_H
