#include "knninterface.h"
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


class KNNInterfacePrivate
{
    KNNInterfacePrivate():maxK(32){}
    friend class KNNInterface;
    int maxK;
};

KNNInterface::KNNInterface(QObject *parent)
    :data(new KNNInterfacePrivate)
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

cv::Mat KNNInterface::startClassification(const cv::Mat &data, const cv::Mat &response)
{
    return data;
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
