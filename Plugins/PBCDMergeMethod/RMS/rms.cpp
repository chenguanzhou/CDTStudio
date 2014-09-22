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

float RMS::merge(QVector<float> buffer)
{
    std::transform(buffer.begin(),buffer.end(),buffer.begin(),[](float &val)->float{
        return val*val;
    });
    float result = std::accumulate(buffer.begin(),buffer.end(),0);
    return sqrt(result/buffer.size());
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(RMS, RMS)
#endif // QT_VERSION < 0x050000
