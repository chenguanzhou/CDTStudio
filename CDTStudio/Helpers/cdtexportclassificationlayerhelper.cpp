#include "cdtexportclassificationlayerhelper.h"
#include "stable.h"

#include "cdtfilesystem.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

bool CDTExportClassificationLayerHelper::exportClassification(QUuid clsID, QString fieldName, QString &shapefilePath)
{
    CDTClassificationLayer *clsLayer =
            CDTClassificationLayer::getLayer(clsID);
    if (clsLayer==NULL || clsLayer->parent()==NULL)
        logger()->error("Classification layer is empty");

    CDTSegmentationLayer *segLayer =
            qobject_cast<CDTSegmentationLayer*>(clsLayer->parent());
    if (segLayer==NULL || segLayer->parent()==NULL)
        logger()->error("Segmentation layer is empty");

    CDTImageLayer *imgLayer =
            qobject_cast<CDTImageLayer*>(segLayer->parent());
    if (imgLayer==NULL)
        logger()->error("Image layer is empty");

    QVariantList data = clsLayer->data();
    QVariantMap clsInfo = clsLayer->clsInfo();

    QMap<QString,QString> categoryID_Name;
    QSqlQuery query(QSqlDatabase::database("category"));
//    if (query.prepare("select id,name from category where imageid=?")==false)
//    {
//        logger()->error("Excuting sql prepare failed:\n %1","select id,name from category where imageid=?");
//        return false;
//    }
//    query.addBindValue(imgLayer->id().toString());
    query.exec(QString("select id,name from category where imageid=\"%1\"").arg(imgLayer->id().toString()));
    while(query.next())
    {
        categoryID_Name.insert(query.value(0).toString(),query.value(1).toString());
    }

    QString shpOriPath;
    QStringList shpOriAffPath;
    if (segLayer->fileSystem()->getFile(segLayer->shapefilePath(),shpOriPath,shpOriAffPath)==false)
        logger()->error("Get shapefile from segmentation layer failed");

    shpOriAffPath<<shpOriPath;
    foreach (QString filePath, shpOriAffPath) {
        QFileInfo file(shapefilePath);
        QString newFilePath = file.dir().path() + "/" + file.completeBaseName() + "." + QFileInfo(filePath).suffix();
        if (QFile::copy(filePath,newFilePath)==false)
            logger()->error(QString("Copy file from %1 to %2 failed!").arg(filePath).arg(newFilePath));
    }

    QgsVectorLayer layer(shapefilePath,QFileInfo(shapefilePath).completeBaseName(),"ogr");
    if (layer.isValid()==false)
        logger()->error("Layer is invalid");

    int index = layer.fieldNameIndex(fieldName);
    if(index==-1)
    {
        if (layer.dataProvider()->addAttributes((QList<QgsField>()<<QgsField(fieldName,QVariant::String)))==false)
            logger()->error("Add attribute failed!");
        index = layer.fieldNameIndex(fieldName);
    }

    layer.startEditing();
    QgsFeatureIterator iter = layer.getFeatures();
    QgsFeature f;
    while(iter.nextFeature(f))
    {
        int index = data[f.attribute("GridCode").toInt()].toInt();
        QString categoryID = clsInfo.key(index);
        QString categoryName = categoryID_Name.value(categoryID);

        if (f.setAttribute(fieldName,categoryName)==false)
        {
            logger()->error("Set classification info to the shapefile failed!\ncategoryID:%1",categoryID);
            qDebug()<<fieldName<<categoryID_Name;
            return false;
        }
        layer.updateFeature(f);
    }
    layer.commitChanges();
    return true;
}
