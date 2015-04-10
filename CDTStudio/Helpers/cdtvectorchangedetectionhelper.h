#ifndef CDTVECTORCHANGEDETECTIONHELPER_H
#define CDTVECTORCHANGEDETECTIONHELPER_H

#include "cdtbasethread.h"
#include "log4qt/logger.h"

class CDTVectorCHangeDetectionHelperPrivate;
class CDTVectorChangeDetectionInterface;

class CDTVectorChangeDetectionHelper:public CDTBaseThread
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    CDTVectorChangeDetectionHelper(
            QString imageid_t1,
            QString imageid_t2,
            QString segid_t1,
            QString segid_t2,
            QString clsid_t1,
            QString clsid_t2,
            QString shapefile_t1,
            QString shapefile_t2,
            QString shapefileFieldName_t1,
            QString shapefileFieldName_t2,
            bool isUseLayer_t1,
            bool isUseLayer_t2,
//            QStringList categoryNamesT1,
//            QStringList categoryNamesT2,
            QMap<QString,QString> categoryPairs,
            CDTVectorChangeDetectionInterface *interface);

    bool isValid() const;
    void run();

    QString shapefilePath()const;
private:
    bool addClsInfoToShp(QString imageID,QString segID,QString clsID,QStringList categoryNames,QString fieldName,QString &shapefilePath);
    void createShapefile(QString path);

    CDTVectorCHangeDetectionHelperPrivate *p;
    QString resultShpPath;
    bool valid;

public:
    static const QString DefaultFieldName;
    static const QString DefaultOtherName;
};

#endif // CDTVECTORCHANGEDETECTIONHELPER_H
