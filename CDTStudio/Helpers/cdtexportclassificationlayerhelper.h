#ifndef CDTEXPORTCLASSIFICATIONLAYERHELPER_H
#define CDTEXPORTCLASSIFICATIONLAYERHELPER_H

#include <QUuid>

class CDTExportClassificationLayerHelper
{
public:
    static bool exportClassification(QUuid clsID,QString fieldName,QString &shapefilePath);
};

#endif // CDTEXPORTCLASSIFICATIONLAYERHELPER_H
