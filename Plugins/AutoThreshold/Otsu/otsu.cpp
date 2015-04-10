#include "otsu.h"
#include <numeric>

Otsu::Otsu(QObject *parent) :
    CDTAutoThresholdInterface(parent)
{

}

QString Otsu::methodName() const
{
    return "Otsu";
}

float Otsu::autoThreshold(QVector<int> data)
{
    int totalCount = std::accumulate(data.begin(),data.end(),0);
    int totalSum = 0;
    for(int i=1;i<data.size();++i)
    {
        totalSum += i*data[i];
    }

    int count1 = 0,count2 = totalCount;
    float sum1 = 0,sum2 = totalSum;

    int maxID = 0;
    int maxVal = -1;

    for (int i=0;i<data.size()-1;++i)
    {
        count1 += data[i];
        count2 = totalCount-count1;
        sum1 += data[i]*i;
        sum2 -= data[i]*i;
        float mean1 = sum1/static_cast<float>(count1);
        float mean2 = sum2/static_cast<float>(count2);
        float var1 = var(data,0,i+1,mean1);
        float var2 = var(data,i+1,data.size(),mean2);
        float val = ratio(count1,count2,var1,var2,mean1,mean2);
        if (val>maxVal)
        {
            maxVal = val;
            maxID = i;
        }
    }

    return maxID + 0.5;
}

float Otsu::var(const QVector<int> &data,int from,int to, float mean)
{
    int count = std::accumulate(data.begin()+from,data.begin()+to,0);
    float var = 0;
    for (int i = from;i<to;++i)
    {
        var += (i-mean)*(i-mean)*data[i];
    }
    var /= count;
    return var;
}

float Otsu::ratio(int count1, int count2, float var1, float var2, float mean1, float mean2)
{
    float b = (mean1-mean2)*(mean1-mean2)*count1*count2;
    float w = var1*count1+var2*count2;
    float val = b/w;
    return val;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Otsu, Otsu)
#endif // QT_VERSION < 0x050000
