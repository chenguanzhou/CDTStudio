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

int Otsu::autoThreshold(QVector<int> histogram)
{
    return 0;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Otsu, Otsu)
#endif // QT_VERSION < 0x050000
