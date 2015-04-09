#ifndef ANNINTERFACE_H
#define ANNINTERFACE_H

#include "cdtclassifierinterface.h"

class ANNInterface : public CDTClassifierInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTClassifierInterface" FILE "ANN.json")
#else
    Q_INTERFACES(CDTClassifierInterface)
#endif // QT_VERSION >= 0x050000
    Q_PROPERTY(QString nodes_in_hidden_layers READ nodes WRITE setNodes DESIGNABLE true USER true)

public:    
    ANNInterface(QObject *parent = 0);
    ~ANNInterface();
    QString classifierName() const;
    QString nodes() const;
    void setNodes(QString node);

    cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses);
protected:
    QList<int> nodesInHiddenLayer;
};

#endif // ANNINTERFACE_H
