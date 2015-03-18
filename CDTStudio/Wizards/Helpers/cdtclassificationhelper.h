#ifndef CDTCLASSIFICATIONHELPER_H
#define CDTCLASSIFICATIONHELPER_H

class cv::Mat;
#include <QtCore>

class CDTClassificationHelper
{
public:
    static void dataNormalize(cv::Mat &data, QString method);
    static void pcaTransform(cv::Mat &data, int pcaParam);
    static void getConfusionMatrix(QList<int> label,QMap<int,QString> testSample,QVector<QVector<int> > &exportMatrix);
};

#endif // CDTCLASSIFICATIONHELPER_H
