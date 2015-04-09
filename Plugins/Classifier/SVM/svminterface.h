#ifndef SVMINTERFACE_H
#define SVMINTERFACE_H

#include "cdtclassifierinterface.h"
#include <opencv2/opencv.hpp>

class SVMInterfacePrivate;
class SVMInterface : public CDTClassifierInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTClassifierInterface" FILE "SVM.json")
#else
    Q_INTERFACES(CDTClassifierInterface)
#endif // QT_VERSION >= 0x050000

    Q_PROPERTY(Type type READ type WRITE setType DESIGNABLE true USER true)
    Q_PROPERTY(Kernel kernel READ kernel WRITE setKernel DESIGNABLE true USER true)
    Q_PROPERTY(double degree READ degree WRITE setDegree DESIGNABLE true USER true)
    Q_PROPERTY(double gamma READ gamma WRITE setGamma DESIGNABLE true USER true)
    Q_PROPERTY(double coef0 READ coef0 WRITE setCoef0 DESIGNABLE true USER true)
    Q_PROPERTY(double c READ c WRITE setC DESIGNABLE true USER true)
    Q_PROPERTY(double nu READ nu WRITE setNu DESIGNABLE true USER true)

    Q_ENUMS(Type)
    Q_ENUMS(Kernel)

public:    
    enum Type {C_SVC = cv::SVM::C_SVC,NU_SVC = cv::SVM::NU_SVC};
    enum Kernel {LINEAR = cv::SVM::LINEAR,POLY ,RBF,SIGMOID };


    SVMInterface(QObject *parent = 0);
    ~SVMInterface();
    QString classifierName() const;

    cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses);

    Type type()const;
    Kernel kernel()const;
    double degree() const;
    double gamma() const;
    double coef0() const;
    double c() const;
    double nu() const;

public slots:
    void setType(Type val);
    void setKernel(Kernel val);
    void setDegree(double val);
    void setGamma(double val);
    void setCoef0(double val);
    void setC(double val);
    void setNu(double val);

private:
    cv::SVMParams* data;
};

#endif // SVMINTERFACE_H
