#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "cdtpbcdmergeinterface.h"


class RMS : public CDTPBCDMergeInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTPBCDMergeInterface" FILE "RMS.json")
#else
    Q_INTERFACES(CDTPBCDMergeInterface)
#endif // QT_VERSION >= 0x050000

public:
    RMS(QObject *parent = 0);

    QString methodName()const;
    float merge(QVector<float> buffer);
};

#endif // GENERICPLUGIN_H
