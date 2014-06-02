#include "bayesinterface.h"
#include <limits>

#ifdef Q_OS_WIN
#include <opencv2/core/version.hpp>
#define CV_VERSION_ID   \
    CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#ifdef QT_DEBUG
#define cvLIB(name) \
    "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) \
    "opencv_" name CV_VERSION_ID
#endif
#pragma comment(lib,cvLIB("core"))
#pragma comment(lib,cvLIB("imgproc"))
#pragma comment(lib,cvLIB("highgui"))
#pragma comment(lib,cvLIB("ml"))
#endif

BayesInterface::BayesInterface(QObject *parent)
{

}

QString BayesInterface::classifierName() const
{
    return tr("Normal Bayes Classifier");
}

cv::Mat BayesInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    cv::Mat result(data.rows,1,CV_32FC1);
    cv::NormalBayesClassifier classifier;
    classifier.train(train_data,responses);
    classifier.predict(data,&result);
    return result;
}



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Bayes, BayesInterface)
#endif // QT_VERSION < 0x050000
