#ifndef CDTEXPORTCLASSIFICATIONLAYERHELPER_H
#define CDTEXPORTCLASSIFICATIONLAYERHELPER_H

#include <QUuid>
#include <log4qt/logger.h>

class CDTExportClassificationLayerHelper
{
    LOG4QT_DECLARE_STATIC_LOGGER(logger,CDTExportClassificationLayerHelper)
public:
    static bool exportClassification(QUuid clsID,QString fieldName,QString &shapefilePath);
};

#endif // CDTEXPORTCLASSIFICATIONLAYERHELPER_H
