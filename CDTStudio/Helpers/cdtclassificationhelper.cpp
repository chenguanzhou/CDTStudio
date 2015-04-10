#include "cdtclassificationhelper.h"
#include "stable.h"

void CDTClassificationHelper::dataNormalize(cv::Mat& data,QString method)
{
    int nRows = data.rows;
    int nCols = data.cols;

    if (method==QString("linear"))
    {
        //linear
        for (int j = 0; j < nCols; ++j)
        {
            float minVal = data.at<float>(0, j);
            float maxVal = data.at<float>(0, j);
            float temp;
            for (int i = 0; i < nRows; ++i)
            {
                temp = data.at<float>(i, j);
                if (temp < minVal)
                    minVal = temp;
                if(temp > maxVal)
                    maxVal = temp;
            }

            temp = maxVal - minVal;
            for (int i = 0; i < nRows; ++i)
            {
                data.at<float>(i, j) = (data.at<float>(i, j) - minVal)/temp;
            }
        }
    }
    else if (method==QString("stddev"))
    {
        //stddev
        for (int j = 0; j < nCols; ++j)
        {
            float meanVal = 0;
            float sigmaVal = 0;
            for (int i =0; i < nRows; ++i)
            {
                meanVal += data.at<float>(i, j);
            }
            meanVal /= nRows;

            for (int i = 0; i < nRows; ++i)
            {
                float temp = data.at<float>(i, j) - meanVal;
                sigmaVal += temp * temp;
            }
            sigmaVal = sqrt(sigmaVal/(nRows - 1));

            //normalize
            for (int i = 0; i < nRows; ++i)
            {
                float temp = ((data.at<float>(i, j) - meanVal)/(3*sigmaVal)+1.)/2.;
                if (temp < 0) temp = 0;
                if (temp > 1) temp = 1;
                data.at<float>(i,j) = temp;
            }
        }
    }
    else if (method==QString("calculas"))
    {
        //calculas
        for (int j = 0; j < nCols; ++j)
        {
            std::vector< std::pair<float, int> > temp_feature;
            for (int i = 0; i < nRows; ++i)
            {
                temp_feature.push_back(std::make_pair(data.at<float>(i, j),i));
            }
            std::sort(temp_feature.begin(), temp_feature.end());


            float sumVal = 0;
            for (int i = 0; i < nRows; ++i)
            {
                sumVal += temp_feature[i].first;
            }

            float sum_temp = 0;
            for (int i = 0; i < nRows; ++i)
            {
                sum_temp += temp_feature[i].first;
                data.at<float>(temp_feature[i].second, j) = sum_temp/sumVal;
            }
        }
    }
    else if (method==QString("rank"))
    {
        //rank
        for (int j = 0; j < nCols; ++j)
        {
            std::vector< std::pair<float, int> > temp_feature;
            for (int i = 0; i < nRows; ++i)
            {
                temp_feature.push_back(std::make_pair(data.at<float>(i, j), i));
            }
            std::sort(temp_feature.begin(), temp_feature.end());

            for (int i = 0; i < nRows; ++i)
            {
                data.at<float>(temp_feature[i].second, j) = (float)i/(nRows - 1);
                //                qDebug()<<mat.at<float>(temp_feature[i].second, j);
            }
        }
    }
}

//Data transform function
void CDTClassificationHelper::pcaTransform(cv::Mat& data,int pcaParam)
{
    if (pcaParam>0 && pcaParam<=data.cols)
    {
        cv::PCA pca(data,cv::Mat(),CV_PCA_DATA_AS_ROW,pcaParam);
        data = pca.project(data);
    }
}

void CDTClassificationHelper::getConfusionMatrix(QList<int> label, QMap<int, QString> testSample, QVector<QVector<int> > &exportMatrix)
{
    //TODO Forget
}

