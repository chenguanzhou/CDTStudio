#include "svminterface.h"
#include <limits>

#if CV_MAJOR_VERSION >= 3
class SVMInterfaceProvate{
    friend class SVMInterface;

    SVMInterfaceProvate()
    {
        svm_type = SVMInterface::C_SVC;
        kernel_type = SVMInterface::RBF;
        degree = 0;
        gamma = 1;
        coef0 = 0;
        C= 1;
        nu = 0;
    }

    SVMInterface::Type svm_type;
    SVMInterface::Kernel kernel_type;
    double degree;
    double gamma;
    double coef0;
    double C;
    double nu;
};
#endif

SVMInterface::SVMInterface(QObject *parent)
    :CDTClassifierInterface(parent),
#if CV_MAJOR_VERSION >= 3
      data(new SVMInterfaceProvate())
#else
      data(new cv::SVMParams)
#endif
{
}

SVMInterface::~SVMInterface()
{
    delete data;
}

QString SVMInterface::classifierName() const
{
    return tr("Support Vector Machines");
}

cv::Mat SVMInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    cv::Mat result(data.rows,1,CV_32FC1);
#if CV_MAJOR_VERSION >= 3
    cv::Ptr<cv::ml::SVM> classifier = cv::ml::SVM::create();
    classifier->setType(type());
    classifier->setKernel(kernel());
    classifier->setDegree(degree());
    classifier->setGamma(gamma());
    classifier->setCoef0(coef0());
    classifier->setC(c());
    classifier->setNu(nu());
    classifier->train(train_data,cv::ml::ROW_SAMPLE,responses);
    classifier->predict(data,result);
#else
    cv::SVM classifier;
    classifier.train(train_data,responses,cv::Mat(),cv::Mat(),*(this->data));
    classifier.predict(data,result);
#endif

    return result;
}

SVMInterface::Type SVMInterface::type() const
{
    return static_cast<Type>(data->svm_type);
}

SVMInterface::Kernel SVMInterface::kernel() const
{
    return static_cast<Kernel>(data->kernel_type);
}

double SVMInterface::degree() const
{
    return data->degree;
}

double SVMInterface::gamma() const
{
    return data->gamma;
}

double SVMInterface::coef0() const
{
    return data->coef0;
}

double SVMInterface::c() const
{
    return data->C;
}

double SVMInterface::nu() const
{
    return data->nu;
}

void SVMInterface::setType(SVMInterface::Type val)
{
    data->svm_type = val;
}

void SVMInterface::setKernel(SVMInterface::Kernel val)
{
    data->kernel_type = val;
}

void SVMInterface::setDegree(double val)
{
    data->degree = val;
}

void SVMInterface::setGamma(double val)
{
    data->gamma = val;
}

void SVMInterface::setCoef0(double val)
{
    data->coef0 = val;
}

void SVMInterface::setC(double val)
{
    data->C = val;
}

void SVMInterface::setNu(double val)
{
    data->nu = val;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(SVM, SVMInterface)
#endif // QT_VERSION < 0x050000
