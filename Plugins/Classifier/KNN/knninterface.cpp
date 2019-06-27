#include "knninterface.h"
#include <limits>
#include <opencv2/opencv.hpp>

class KNNInterfacePrivate
{
    KNNInterfacePrivate():maxK(10){}
    friend class KNNInterface;
    int maxK;
};

KNNInterface::KNNInterface(QObject *parent)
    :CDTClassifierInterface(parent),
    data(new KNNInterfacePrivate)
{

}

KNNInterface::~KNNInterface()
{
    delete data;
}

QString KNNInterface::classifierName() const
{
    return tr("K-Nearest Neighbors");
}

cv::Mat KNNInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    cv::Mat result(data.rows,1,CV_32FC1);

#if CV_MAJOR_VERSION >=3
    cv::Ptr<cv::ml::KNearest> classifier = cv::ml::KNearest::create();
    classifier->setDefaultK(maxK());
    classifier->train(train_data,cv::ml::ROW_SAMPLE,responses);
    classifier->findNearest(data,maxK(),result);
#else
    cv::KNearest *classifier = new cv::KNearest();
    classifier->train(train_data,responses,cv::Mat(),false,maxK());
    cv::Mat testSample(1, data.cols, CV_32FC1);
    for (int i=0;i<data.rows;++i)
    {
        for (int j=0; j<data.cols; ++j)
        {
            testSample.at<float>(0, j) = data.at<float>(i,j);
        }
        result.at<float>(i,0) = classifier->find_nearest(testSample,maxK());
    }
    delete classifier;
#endif

    return result;
}

int KNNInterface::maxK() const
{
    return data->maxK;
}

void KNNInterface::setMaxK(int k)
{
    data->maxK = k;
}



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(KNN, KNNInterface)
#endif // QT_VERSION < 0x050000
