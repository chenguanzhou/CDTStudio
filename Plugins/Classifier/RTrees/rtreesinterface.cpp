#include "rtreesinterface.h"
#include <limits>
#include <opencv2/opencv.hpp>

RTreesInterface::RTreesInterface(QObject *parent)
    :CDTClassifierInterface(parent)
{
}

QString RTreesInterface::classifierName() const
{
    return tr("Random Trees");
}

cv::Mat RTreesInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    cv::Mat result(data.rows,1,CV_32FC1);
    cv::RandomTrees classifier;

    cv::RandomTreeParams param;
    param.min_sample_count = 4;
    param.max_categories = 5;

    classifier.train(train_data,CV_ROW_SAMPLE,responses,cv::Mat(),cv::Mat(),cv::Mat(),cv::Mat(),param);

    cv::Mat testSample(1, data.cols, CV_32FC1);
    for (int i=0;i<data.rows;++i)
    {
        for (int j=0; j<data.cols; ++j)
        {
            testSample.at<float>(0, j) = data.at<float>(i,j);
        }
        result.at<float>(i,0) = classifier.predict(testSample);
    }
    return result;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(RTrees, RTreesInterface)
#endif // QT_VERSION < 0x050000
