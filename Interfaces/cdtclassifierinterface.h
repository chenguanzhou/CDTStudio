#ifndef CDTCLASSIFIERINTERFACE_H
#define CDTCLASSIFIERINTERFACE_H

#include <QtCore>
#include <QtPlugin>
#include <opencv2/opencv.hpp>

class CDTClassifierInterface:public QObject
{
    Q_OBJECT    
public:
    explicit CDTClassifierInterface(QObject* parent = 0):QObject(parent){}    

    virtual QString classifierName() const = 0;
    virtual cv::Mat startClassification(const cv::Mat &data,const cv::Mat &response) = 0;
signals:        

private:    
};

Q_DECLARE_INTERFACE(CDTClassifierInterface,"cn.edu.WHU.CDTStudio.CDTClassifierInterface/1.0")

#endif // CDTCLASSIFIERINTERFACE_H
