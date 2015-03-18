#include "cdtvectorchangedetectionhelper.h"
#include <qgsvectorlayer.h>
#include "cdtvectorchangedetectioninterface.h"

class CDTVectorCHangeDetectionHelperPrivate
{
    friend class CDTVectorCHangeDetectionHelper;

    CDTVectorCHangeDetectionHelperPrivate (
            QString shapefilePathT1,
            QString shapefilePathT2,
            QString shapefilePathResult,
            CDTVectorChangeDetectionInterface *interface,
            QString fieldNameT1,
            QString fieldNameT2)
        : layerT1(NULL),
          layerT2(NULL),
          layerResult(NULL),
          plugin(interface)
    {
        layerT1 = new QgsVectorLayer(shapefilePathT1,QFileInfo(shapefilePathT1).completeBaseName(),"ogr");
        layerT2 = new QgsVectorLayer(shapefilePathT2,QFileInfo(shapefilePathT1).completeBaseName(),"ogr");
        layerResult = new QgsVectorLayer(shapefilePathResult,QFileInfo(shapefilePathT1).completeBaseName(),"ogr");
        this->fieldNameT1 = fieldNameT1;
        this->fieldNameT2 = fieldNameT2;
    }

    bool isValid()const
    {
        if (layerT1==NULL || layerT2==NULL ||
                layerResult == NULL || plugin==NULL)
            return false;
        else
            return layerT1->isValid() &&
                    layerT2->isValid() &&
                    layerResult->isValid();
    }

    QgsVectorLayer *layerT1;
    QgsVectorLayer *layerT2;
    QgsVectorLayer *layerResult;
    CDTVectorChangeDetectionInterface *plugin;
    QString fieldNameT1,fieldNameT2;
};

CDTVectorCHangeDetectionHelper::CDTVectorCHangeDetectionHelper(
        QString shapefilePathT1,
        QString shapefilePathT2,
        QString shapefilePathResult,
        QString fieldNameT1,
        QString fieldNameT2,
        CDTVectorChangeDetectionInterface *interface)
    :p(new CDTVectorCHangeDetectionHelperPrivate(
           shapefilePathT1,
           shapefilePathT2,
           shapefilePathResult,
           interface,
           fieldNameT1,
           fieldNameT2))
{    
}

bool CDTVectorCHangeDetectionHelper::isValid() const
{
    return p->isValid();
}

void CDTVectorCHangeDetectionHelper::run()
{
    try
    {
        if (isValid()==false) throw tr("Params of detection is invalid!");

        p->plugin->detect(p->layerT1,p->layerT2,p->layerResult,p->fieldNameT1,p->fieldNameT2);
    }
    catch(QString msg)
    {
        logger()->error(msg);
    }
}
