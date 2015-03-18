#ifndef CDTVECTORCHANGEDETECTIONHELPER_H
#define CDTVECTORCHANGEDETECTIONHELPER_H

#include "cdtbasethread.h"
#include "log4qt/logger.h"

class CDTVectorCHangeDetectionHelperPrivate;
class CDTVectorChangeDetectionInterface;

class CDTVectorCHangeDetectionHelper:public CDTBaseThread
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    CDTVectorCHangeDetectionHelper(
            QString shapefilePathT1,
            QString shapefilePathT2,
            QString shapefilePathResult,
            QString fieldNameT1,
            QString fieldNameT2,
            CDTVectorChangeDetectionInterface *interface);

    bool isValid() const;
    void run();
private:
    CDTVectorCHangeDetectionHelperPrivate *p;
};

#endif // CDTVECTORCHANGEDETECTIONHELPER_H
