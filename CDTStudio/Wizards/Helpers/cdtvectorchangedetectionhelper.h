#ifndef CDTVECTORCHANGEDETECTIONHELPER_H
#define CDTVECTORCHANGEDETECTIONHELPER_H

#include "cdtbasethread.h"

class CDTVectorCHangeDetectionHelperPrivate;
class CDTVectorChangeDetectionInterface;

class CDTVectorCHangeDetectionHelper:public CDTBaseThread
{
   Q_OBJECT
public:
    CDTVectorCHangeDetectionHelper(QString shapefilePathT1,
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
