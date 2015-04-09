#include "autosvminterface.h"
#include <limits>
#include <opencv2/opencv.hpp>

AutoSVMInterface::AutoSVMInterface(QObject *parent)
    :CDTClassifierInterface(parent)
{
}

AutoSVMInterface::~AutoSVMInterface()
{
}

QString AutoSVMInterface::classifierName() const
{
    return tr("Auto Training Support Vector Machines");
}

cv::Mat AutoSVMInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    cv::Mat result(data.rows,1,CV_32FC1);
    cv::SVM classifier;

    classifier.train_auto(train_data,responses,cv::Mat(),cv::Mat(),cv::SVMParams());
    classifier.predict(data,result);

    return result;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(AutoSVM, AutoSVMInterface)
#endif // QT_VERSION < 0x050000
