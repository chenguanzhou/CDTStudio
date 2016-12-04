#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "cdtautothresholdinterface.h"


class Otsu : public CDTAutoThresholdInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTAutoThresholdInterface" FILE "Otsu.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(CDTAutoThresholdInterface)

public:
    Otsu(QObject *parent = 0);

    QString methodName()const;
    float autoThreshold(QVector<int> data);

private:
//    float var(const QVector<int> &data, int from, int to, float mean);
    double ratio(int count1, int count2, double mean1, double mean2);
};

#endif // GENERICPLUGIN_H
