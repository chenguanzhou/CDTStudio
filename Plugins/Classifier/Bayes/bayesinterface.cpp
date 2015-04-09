#include "bayesinterface.h"
#include <limits>
#include <opencv2/opencv.hpp>

BayesInterface::BayesInterface(QObject *parent)
    :CDTClassifierInterface(parent)
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
