#include "mstmethodinterface.h"

class MSTMethodPrivate
{
public:
    MSTMethodPrivate():threshold(20),minObjectCount(100),shieldNulValue(false){}
    double threshold;
    int minObjectCount;
    bool shieldNulValue;
};

MSTMethodInterface::MSTMethodInterface(QObject *parent)
    :CDTSegmentationInterface(parent),
      pData(new MSTMethodPrivate)
{
}

MSTMethodInterface::~MSTMethodInterface()
{
}

QString MSTMethodInterface::segmentationMethod() const
{
    return QString("mst");
}

void MSTMethodInterface::startSegmentation()
{

}

double MSTMethodInterface::threshold() const
{
    return pData->threshold;
}

int MSTMethodInterface::minObjectCount() const
{
    return pData->minObjectCount;
}

bool MSTMethodInterface::shieldNulValue() const
{
    return pData->shieldNulValue;
}

void MSTMethodInterface::setThreshold(double val)
{
    pData->threshold = val;
}

void MSTMethodInterface::setMinObjectCount(int val)
{
    pData->minObjectCount = val;
}

void MSTMethodInterface::setShieldNulValue(bool val)
{
    pData->shieldNulValue = val;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MSTMethod, MSTMethodInterface)
#endif // QT_VERSION < 0x050000
