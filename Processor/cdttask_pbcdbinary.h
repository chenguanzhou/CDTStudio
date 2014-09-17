#ifndef CDTTASK_PBCDBINARY_H
#define CDTTASK_PBCDBINARY_H

#include "cdttask.h"
class CDTTask_PBCDBinary : public CDTTask
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit CDTTask_PBCDBinary(QString id, QDomElement params, QObject *parent = 0);

signals:

public slots:
    void start();
};

#endif // CDTTASK_PBCDBINARY_H
