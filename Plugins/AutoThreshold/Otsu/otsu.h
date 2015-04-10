#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "cdtautothresholdinterface.h"


class Otsu : public CDTAutoThresholdInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTAutoThresholdInterface" FILE "RMS.json")
#else
    Q_INTERFACES(CDTAutoThresholdInterface)
#endif // QT_VERSION >= 0x050000

public:
    Otsu(QObject *parent = 0);

    QString methodName()const;
    float autoThreshold(QVector<int> data);

private:
    float var(const QVector<int> &data, int from, int to, float mean);
    float ratio(int count1,int count2,float var1,float var2,float mean1,float mean2);
};

#endif // GENERICPLUGIN_H
