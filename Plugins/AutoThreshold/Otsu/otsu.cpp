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
//    int totalCount = std::accumulate(data.begin(),data.end(),0);
//    double totalSum = 0;
    qint64 totalCount = 0;
    double totalSum = 0.;
    for(int i=0;i<data.size();++i)
    {
        totalCount += data[i];
        totalSum += i*static_cast<double>(data[i]);
    }

    qint64 count1 = 0,count2 = totalCount;
    double sum1 = 0,sum2 = totalSum;

    int maxID = 0;
    double maxVal = -1;

    for (int i=0;i<data.size()-1;++i)
    {
        count1 += data[i];
        if (count1==0)
            continue;

        count2 = totalCount-count1;

        if (count2==0)
            break;

        sum1 += static_cast<double>(data[i])*i;
        sum2 -= static_cast<double>(data[i])*i;
        double mean1 = sum1/static_cast<double>(count1);
        double mean2 = sum2/static_cast<double>(count2);
//        float var1 = var(data,0,i+1,mean1);
//        float var2 = var(data,i+1,data.size(),mean2);
        double val = ratio(count1,count2/*,var1,var2*/,mean1,mean2);
        if (val>maxVal)
        {
            maxVal = val;
            maxID = i;
        }
//        qDebug()<<i<<mean1<<mean2<<count2<<sum2<<maxID;
    }
    return maxID + 0.5;
}

//float Otsu::var(const QVector<int> &data,int from,int to, float mean)
//{
//    int count = std::accumulate(data.begin()+from,data.begin()+to,0);
//    float var = 0;
//    for (int i = from;i<to;++i)
//    {
//        var += (i-mean)*(i-mean)*data[i];
//    }
//    var /= count;
//    return var;
//}

double Otsu::ratio(int count1, int count2, /*float var1, float var2,*/ double mean1, double mean2)
{
    double b = (mean1-mean2)*(mean1-mean2)*count1*count2;
//    float w = var1*count1+var2*count2;
//    float val = b/w;
//    return val;
    return b;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Otsu, Otsu)
#endif // QT_VERSION < 0x050000
