#include "cdtvectorchangedetectionhelper.h"
#include "stable.h"
#include "cdtvectorchangedetectioninterface.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"
#include "cdtfilesystem.h"


class CDTVectorCHangeDetectionHelperPrivate
{
    friend class CDTVectorChangeDetectionHelper;

    CDTVectorCHangeDetectionHelperPrivate (
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
            QMap<QString, QString> pairs,
            CDTVectorChangeDetectionInterface *interface)
        : plugin(interface)
    {
        this->imageid_t1 = imageid_t1;
        this->imageid_t2 = imageid_t2;
        this->segid_t1 = segid_t1;
        this->segid_t2 = segid_t2;
        this->clsid_t1 = clsid_t1;
        this->clsid_t2 = clsid_t2;
        this->shapefile_t1 = shapefile_t1;
        this->shapefile_t2 = shapefile_t2;
        this->shapefileFieldName_t1 = shapefileFieldName_t1;
        this->shapefileFieldName_t2 = shapefileFieldName_t2;
        this->isUseLayer_t1 = isUseLayer_t1;
        this->isUseLayer_t2 = isUseLayer_t2;
        this->categoryNamesT1 = pairs.keys();
        this->categoryNamesT2 = pairs.values();
        this->pairs = pairs;
    }

    QString imageid_t1;
    QString imageid_t2;
    QString segid_t1;
    QString segid_t2;
    QString clsid_t1;
    QString clsid_t2;
    QString shapefile_t1;
    QString shapefile_t2;
    QString shapefileFieldName_t1;
    QString shapefileFieldName_t2;
    bool isUseLayer_t1;
    bool isUseLayer_t2;
    QStringList categoryNamesT1;
    QStringList categoryNamesT2;
    QMap<QString, QString> pairs;
    CDTVectorChangeDetectionInterface *plugin;
};

const QString CDTVectorChangeDetectionHelper::DefaultFieldName = "category";
const QString CDTVectorChangeDetectionHelper::DefaultOtherName = CDTVectorChangeDetectionHelper::tr("others");

CDTVectorChangeDetectionHelper::CDTVectorChangeDetectionHelper(QString imageid_t1,
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
//        QStringList categoryNamesT1,
//        QStringList categoryNamesT2,
        QMap<QString, QString> categoryPairs,
        CDTVectorChangeDetectionInterface *interface)
    :valid(false),
      p(new CDTVectorCHangeDetectionHelperPrivate(
            imageid_t1,
            imageid_t2,
            segid_t1,
            segid_t2,
            clsid_t1,
            clsid_t2,
            shapefile_t1,
            shapefile_t2,
            shapefileFieldName_t1,
            shapefileFieldName_t2,
            isUseLayer_t1,
            isUseLayer_t2,
//            categoryNamesT1,
//            categoryNamesT2,
            categoryPairs,
            interface))
{        
}



bool CDTVectorChangeDetectionHelper::isValid() const
{
    return valid;
}

void CDTVectorChangeDetectionHelper::run()
{
    try
    {
        valid = false;
        if (p->isUseLayer_t1)
        {
            emit currentProgressChanged(tr("Add category information to the shapefile of segmentation layer 1"));
            valid = addClsInfoToShp(
                        p->imageid_t1,
                        p->segid_t1,
                        p->clsid_t1,
                        p->categoryNamesT1,
                        DefaultFieldName,
                        p->shapefile_t1);
            if (valid == false)
                return;
            p->shapefileFieldName_t1 = DefaultFieldName;
        }
        if (p->isUseLayer_t2)
        {
            emit currentProgressChanged(tr("Add category information to the shapefile of segmentation layer 2"));
            valid = addClsInfoToShp(
                        p->imageid_t2,
                        p->segid_t2,
                        p->clsid_t2,
                        p->categoryNamesT2,
                        DefaultFieldName,
                        p->shapefile_t2);
            if (valid == false) return;
            p->shapefileFieldName_t2 = DefaultFieldName;
        }

        resultShpPath = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".shp";
        createShapefile(resultShpPath);

        auto isLayerValid = [](QgsVectorLayer *l)
        {
            if (l==NULL) throw "Layer is null";
            if (l->isValid()==false) throw "Layer is invalid";
        };
        QgsVectorLayer *layerT1 = new QgsVectorLayer(p->shapefile_t1,QFileInfo(p->shapefile_t1).completeBaseName(),"ogr");
        QgsVectorLayer *layerT2 = new QgsVectorLayer(p->shapefile_t2,QFileInfo(p->shapefile_t2).completeBaseName(),"ogr");
        QgsVectorLayer *layerResult = new QgsVectorLayer(resultShpPath,QFileInfo(resultShpPath).completeBaseName(),"ogr");
        try{
            isLayerValid(layerT1);
            isLayerValid(layerT2);
            isLayerValid(layerResult);
        }
        catch(QString msg){
            valid = false;
            return;
        }

        emit currentProgressChanged(tr("Start detection"));
        connect(p->plugin,SIGNAL(currentProgressChanged(QString)),this,SIGNAL(currentProgressChanged(QString)));
        connect(p->plugin,SIGNAL(progressBarValueChanged(int)),this,SIGNAL(progressBarValueChanged(int)));
        p->plugin->detect(
                    layerT1,
                    layerT2,
                    layerResult,
                    p->shapefileFieldName_t1,
                    p->shapefileFieldName_t2,
                    p->pairs);
        disconnect(p->plugin,SIGNAL(currentProgressChanged(QString)),this,SIGNAL(currentProgressChanged(QString)));
        disconnect(p->plugin,SIGNAL(progressBarValueChanged(int)),this,SIGNAL(progressBarValueChanged(int)));
        emit currentProgressChanged(tr("Completed!"));
        valid = true;
    }
    catch(QString msg)
    {
        logger()->error(msg);
    }
}

QString CDTVectorChangeDetectionHelper::shapefilePath() const
{
    return resultShpPath;
}

bool CDTVectorChangeDetectionHelper::addClsInfoToShp(QString imageID, QString segID, QString clsID, QStringList categoryNames, QString fieldName, QString &shapefilePath)
{
    CDTSegmentationLayer *segLayer =
            CDTSegmentationLayer::getLayer(segID);
    CDTClassificationLayer *clsLayer =
            CDTClassificationLayer::getLayer(clsID);

    if (segLayer==NULL || segLayer==NULL )
        return false;

    //Get categoryID_categoryName
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select id,name from category where name = ? and imageid=?");
    QMap<QString,QString> categoryID_Name;
    foreach (QString name, categoryNames) {
        query.addBindValue(name);
        query.addBindValue(imageID);
        query.exec();
        query.next();
        categoryID_Name.insert(query.value(0).toString(),
                               query.value(1).toString());
    }

    //Get category name for every object
    QVariantList data = clsLayer->data();
    QVariantMap clsInfo = clsLayer->clsInfo();

    QMap<int,QString> id_Name;
    foreach (QString key, clsInfo.keys()) {
        int value = clsInfo.value(key).toInt();
        if (categoryID_Name.keys().contains(key))
            id_Name.insert(value,categoryID_Name[key]);
        else
            id_Name.insert(value,DefaultOtherName);
    }

    QStringList nameList;
    foreach (QVariant id, data) {
        nameList.push_back(id_Name[id.toInt()]);
    }

    //Write them to the shapefile
    segLayer->fileSystem()->getFile(segLayer->shapefilePath(),shapefilePath);
    QgsVectorLayer layer(shapefilePath,QFileInfo(shapefilePath).completeBaseName(),"ogr");
    if (layer.isValid()==false)
    {
        logger()->error(layer.error().message(QgsErrorMessage::Text));
        return false;
    }

    int index = layer.fieldNameIndex(fieldName);
    if(index==-1)
    {
        if (layer.dataProvider()->addAttributes((QList<QgsField>()<<QgsField(fieldName,QVariant::String)))==false)
        {
            logger()->warn("Add attribute failed!");
            return false;
        }
        index = layer.fieldNameIndex(fieldName);
    }

    layer.startEditing();

    QgsFeatureIterator iter = layer.getFeatures();
    QgsFeature f;
    long count = layer.featureCount();
    int gap = count/100+1;
    int i=0;
    emit progressBarValueChanged(0);
    while(iter.nextFeature(f))
    {
        QString name = nameList[f.attribute("GridCode").toInt()];
        if (f.setAttribute(fieldName,name)==false)
        {
            logger()->error("Set classification info to the shapefile failed!");
            return false;
        }
        layer.updateFeature(f);
        i ++;
        if (i%gap==0) emit progressBarValueChanged(i*100/count);
    }
    layer.commitChanges();
    emit progressBarValueChanged(100);

    return true;
}

void CDTVectorChangeDetectionHelper::createShapefile(QString path)
{
    GDALAllRegister();
    OGRRegisterAll();


    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    Q_ASSERT(poDriver);
    OGRDataSource* poDS = poDriver->CreateDataSource(path.toUtf8().constData(),NULL);
    Q_ASSERT(poDS);
//    OGRSpatialReference *reference = new OGRSpatialReference(poImageDS->GetProjectionRef());
    OGRLayer *layer = poDS->CreateLayer("change",NULL,wkbPolygon,NULL);
    Q_ASSERT(layer);

    OGRFieldDefn fieldBefore( "before", OFTString );
    if( layer->CreateField( &fieldBefore ) != OGRERR_NONE )
    {
        logger()->error( "Creating field failed.") ;
        return ;
    }
    OGRFieldDefn fieldAfter( "after", OFTString );
    if( layer->CreateField( &fieldAfter ) != OGRERR_NONE )
    {
        logger()->error( "Creating field failed.") ;
        return ;
    }
    OGRFieldDefn fieldIsChangedd( "ischanged", OFTString );
    if( layer->CreateField( &fieldIsChangedd ) != OGRERR_NONE )
    {
        logger()->error( "Creating field failed.") ;
        return ;
    }
    OGRDataSource::DestroyDataSource(poDS);
}
