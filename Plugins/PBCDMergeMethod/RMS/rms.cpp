#include "RMS.h"
#include <numeric>

RMS::RMS(QObject *parent) :
    CDTPBCDMergeInterface(parent)
{

}

QString RMS::methodName() const
{
    return "Root Mean Square";
}

float RMS::merge(QVector<float>::const_iterator begin, QVector<float>::const_iterator end)
{
    int count = 0;
    float result = 0;
    std::for_each(begin,end,[&](const float &val){
        result += val*val;
        ++count;
    });
//    float result = std::accumulate(begin.begin(),begin.begin()+end,0);
    return sqrt(result/count);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(RMS, RMS)
#endif // QT_VERSION < 0x050000
