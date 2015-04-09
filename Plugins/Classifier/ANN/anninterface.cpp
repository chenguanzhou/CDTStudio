#include "anninterface.h"
#include <limits>
#include <opencv2/opencv.hpp>

ANNInterface::ANNInterface(QObject *parent)
    :CDTClassifierInterface(parent)
{
    nodesInHiddenLayer.push_back(10);
}

ANNInterface::~ANNInterface()
{
}

QString ANNInterface::classifierName() const
{
    return tr("Artificial Neural Network");
}

QString ANNInterface::nodes() const
{
    QStringList nodes;
    foreach (int n, nodesInHiddenLayer) {
        nodes.push_back(QString::number(n));
    }
    return nodes.join(",");
}

void ANNInterface::setNodes(QString node)
{
    QList<int> nodes;
    QStringList nodesList = node.split(",");
    foreach (QString s, nodesList) {
        bool ret = true;
        int num = s.toInt(&ret);
        if (ret == false) return;
        if (num<1) return;
        nodes.push_back(num);
    }
    nodesInHiddenLayer = nodes;
}

cv::Mat ANNInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    //Make layers information
    QSet<int> categoriesInfo;
    for (int i=0;i<responses.rows;++i)
    {
        categoriesInfo.insert(responses.at<int>(i,0));
    }
    QList<int> totalNodes;
    totalNodes<<data.cols;
    totalNodes+=nodesInHiddenLayer;
    totalNodes<<categoriesInfo.size();

    cv::Mat layerSizes = cv::Mat_<int>(1,totalNodes.size());
    for(int i=0;i<totalNodes.size();++i)
    {
        layerSizes.at<int>(0,i) = totalNodes[i];
    }

    //Make suitable labels for ANN
    cv::Mat newResponses = cv::Mat_<float>::zeros(responses.rows,categoriesInfo.size());
    for(int i = 0;i<newResponses.rows;++i)
    {
        newResponses.at<float>(i,responses.at<int>(i,0)) = 1;
    }

    //Create classifier
    CvANN_MLP classifier;
    classifier.create(layerSizes);
    classifier.train(train_data,newResponses,cv::Mat());

    //predict
    cv::Mat result = cv::Mat_<float>(data.rows,categoriesInfo.size());
    cv::Mat resultReturn = cv::Mat_<float>(data.rows,1);
    classifier.predict(data,result);
    for (int i=0; i<result.rows;++i)
    {
        float* rowData = result.ptr<float>(i);
        float max = rowData[0];
        size_t id = 0;
        for (int j=1;j<categoriesInfo.size();++j)
        {
            if (rowData[j]>max)
            {
                max = rowData[j];
                id = j;
            }
        }
        resultReturn.at<float>(i,0) = id;
    }
    std::cout<<result<<std::endl;
    std::cout<<resultReturn<<std::endl;

//    cv::Mat layerSizes;
//    layerSizes;
//    classifier.create();

    return resultReturn;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ANN, ANNInterface)
#endif // QT_VERSION < 0x050000
