#include "cdtimagelayer.h"
#include "stable.h"
#include <qgsmultibandcolorrenderer.h>
#include <qgscontrastenhancement.h>
#include <qgscontrastenhancementfunction.h>
#include "mainwindow.h"
#include "cdtprojecttreeitem.h"
#include "cdtprojectlayer.h"
#include "cdtextractionlayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtfilesystem.h"
#include "dialognewextraction.h"
#include "dialognewsegmentation.h"
#include "dialogdbconnection.h"

QList<CDTImageLayer *> CDTImageLayer::layers;

CDTImageLayer::CDTImageLayer(QUuid uuid, QObject *parent)
    : CDTBaseLayer(uuid,parent),
      multibandSelectionWidget(NULL),
      enhancementStyle(QgsContrastEnhancement::NoEnhancement),
      useRelative(false)
{
    setKeyItem(new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE,CDTProjectTreeItem::RASTER,QString(),this));
    extractionRoot
            = new CDTProjectTreeItem(CDTProjectTreeItem::EXTRACTION_ROOT,CDTProjectTreeItem::GROUP,tr("Extractions"),this);
    segmentationsRoot
            = new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,CDTProjectTreeItem::GROUP,tr("Segmentations"),this);
    standardKeyItem()->appendRow(extractionRoot);
    standardKeyItem()->appendRow(segmentationsRoot);

    layers.push_back(this);

    connect(this,SIGNAL(removeImageLayer(CDTImageLayer*)),(CDTProjectLayer*)(this->parent()),SLOT(removeImageLayer(CDTImageLayer*)));


    //actions
//    QWidgetAction *actionMultiBandRenderer  = new QWidgetAction(this);

    QAction *actionRename                   = new QAction(QIcon(":/Icons/Rename.png"),tr("Rename Image"),this);
    QAction *actionRemoveImage              = new QAction(QIcon(":/Icons/Remove.png"),tr("Remove Image"),this);
    QAction *actionAddExtractionLayer       = new QAction(QIcon(":/Icons/Add.png"),tr("Add Extraction"),this);
    QAction *actionAddSegmentationLayer     = new QAction(QIcon(":/Icons/Add.png"),tr("Add Segmentation"),this);
    QAction *actionRemoveAllExtractions     = new QAction(QIcon(":/Icons/Remove.png"),tr("Remove All Extractions"),this);
    QAction *actionRemoveAllSegmentations   = new QAction(QIcon(":/Icons/Remove.png"),tr("Remove All Segmentations"),this);

    setActions(QList<QList<QAction *> >()
               <<(QList<QAction *>()/*<<actionOpacity<<actionMultiBandRenderer*/<<actionRename<<actionRemoveImage)
               <<(QList<QAction *>()<<actionAddExtractionLayer<<actionRemoveAllExtractions)
               <<(QList<QAction *>()<<actionAddSegmentationLayer<<actionRemoveAllSegmentations));


//    Multi-band Selection
//    multibandSelectionWidget = new QWidget(NULL);
//    actionMultiBandRenderer->setDefaultWidget(multibandSelectionWidget);




    connect(actionRename,SIGNAL(triggered()),this,SLOT(rename()));
    connect(actionRemoveImage,SIGNAL(triggered()),this,SLOT(remove()));
    connect(actionAddSegmentationLayer,SIGNAL(triggered()),this,SLOT(addSegmentation()));
    connect(actionRemoveAllSegmentations,SIGNAL(triggered()),this,SLOT(removeAllSegmentationLayers()));
    connect(actionAddExtractionLayer,SIGNAL(triggered()),this,SLOT(addExtraction()));
    connect(actionRemoveAllExtractions,SIGNAL(triggered()),this,SLOT(removeAllExtractionLayers()));
}

CDTImageLayer::~CDTImageLayer()
{
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from imagelayer where id = '"+id().toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
    ret = query.exec("delete from category where imageID = '"+id().toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    query.exec("select id from image_validation_samples where imageID = '"+id().toString()+"'");
    QStringList list;
    while (query.next())
    {
        list.push_back(query.value(0).toString());
    }
    foreach (QString string, list) {
        ret = query.exec("delete from point_category where validationid = '"+string+"'");
        if (!ret)
            qWarning()<<"prepare:"<<query.lastError().text();
    }
    ret = query.exec("delete from image_validation_samples where imageID = '"+id().toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
    layers.removeAll(this);
}

void CDTImageLayer::initLayer(const QString &name, const QString &path)
{
    useRelative = QFileInfo(path).isRelative();

    CDTProjectLayer *layer = qobject_cast<CDTProjectLayer *>(parent());
    QDir dir(layer->path());
    QString absoluteFilePath = dir.absoluteFilePath(path);
    QgsRasterLayer *newCanvasLayer = new QgsRasterLayer(absoluteFilePath,QFileInfo(path).completeBaseName());
    if (!newCanvasLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open image ")+absoluteFilePath+tr(" failed!"));
        delete newCanvasLayer;
        return;
    }

    keyItem()->setText(name);
    keyItem()->setToolTip(path);

    setCanvasLayer(newCanvasLayer);

    QSqlDatabase db = QSqlDatabase::database("category");
    if (db.isValid()==false)
    {
        QMessageBox::critical(NULL,tr("Error"),tr("database is invalid"));
        return ;
    }
    QSqlQuery query(db);
    bool ret ;
    ret = query.prepare("insert into imagelayer VALUES(?,?,?,?)");
    if (ret == false)
    {
        QMessageBox::critical(NULL,tr("Error"),tr("insert image layer failed!\nerror:")+query.lastError().text());
        return ;
    }
    query.bindValue(0,id().toString());
    query.bindValue(1,name);
    query.bindValue(2,path);
    query.bindValue(3,((CDTProjectLayer*)parent())->id().toString());
    query.exec();

    QList<QPair<QLabel*,QWidget*>> widgets;
    //Opacity slider
    QSlider *slider = new QSlider();
    slider->setOrientation(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setMaximum(100);
    slider->setValue(100);
    connect(slider,SIGNAL(valueChanged(int)),SLOT(setLayerOpacity(int)));
    connect(this,SIGNAL(destroyed()),slider,SLOT(deleteLater()));
    widgets<<qMakePair(new QLabel(tr("Opacity")),(QWidget*)slider);

    //Enhancement
    QComboBox *comboEnhancement = new QComboBox();
    comboEnhancement->addItems(QStringList()
                               <<tr("No Enhancement")
                               <<tr("Stretch To Min/Max")
                               <<tr("Stretch And Clip To Min/Max")
                               <<tr("Clip To Min/Max"));    
    connect(comboEnhancement,SIGNAL(currentIndexChanged(int)),SLOT(onEnhancementChanged(int)));
    widgets<<qMakePair(new QLabel(tr("Enhancement")),(QWidget*)comboEnhancement);

    //Init multiband selection widget
    if (newCanvasLayer->bandCount()>=3)
    {
        QStringList bandTexts = QStringList()<<tr("Red")<<tr("Green")<<tr("Blue");
        QList<QColor> bandColors = QList<QColor>()<<QColor(Qt::red)<<QColor(Qt::green)<<QColor(Qt::blue);
        QStringList bandNames;
        for(int i=0;i<newCanvasLayer->bandCount();++i)
            bandNames<<tr("Band %1").arg(i+1);

        QList<QComboBox*> comboList;
        const int rgbCount = 3;
        for(int i=0;i<rgbCount;++i)
        {
            QLabel *label = new QLabel(bandTexts[i]);
            QPalette palette = label->palette();
            palette.setColor(QPalette::WindowText,bandColors[i]);
            label->setPalette(palette);
            QComboBox *combo = new QComboBox();
            combo->addItems(bandNames);
            combo->setCurrentIndex(i);
            comboList << combo;
            widgets<<qMakePair(label,(QWidget*)combo);
        }

        rBandID = 1;gBandID = 2;bBandID = 3;

        connect(comboList[0],SIGNAL(currentIndexChanged(int)),SLOT(redBandChanged(int)));
        connect(comboList[1],SIGNAL(currentIndexChanged(int)),SLOT(greenBandChanged(int)));
        connect(comboList[2],SIGNAL(currentIndexChanged(int)),SLOT(blueBandChanged(int)));
    }

    setWidgetActions(widgets);

    if (newCanvasLayer->dataProvider()->dataType(1) != QGis::Byte)
    {
        newCanvasLayer->setContrastEnhancement(QgsContrastEnhancement::StretchToMinimumMaximum,QgsRaster::ContrastEnhancementCumulativeCut,QgsRectangle(),0);
        comboEnhancement->setCurrentIndex(1);
    }
    else
        comboEnhancement->setCurrentIndex(0);

    emit appendLayers(QList<QgsMapLayer*>()<<canvasLayer());
    emit layerChanged();
}

void CDTImageLayer::setCategoryInfo(const CDTCategoryInformationList &info)
{
    QSqlDatabase db = QSqlDatabase::database("category");
    if (db.isValid()==false)
    {
        QMessageBox::critical(NULL,tr("Error"),tr("database is invalid"));
        return ;
    }
    QSqlQuery query(db);
    bool ret ;

    ret = query.prepare("insert into category VALUES(?,?,?,?)");
    if (ret == false)
    {
        QMessageBox::critical(NULL,tr("Error"),tr("insert data failed!\nerror:")+query.lastError().text());
        return ;
    }

    foreach (CategoryInformation inf, info) {
        query.bindValue(0,inf.id.toString());
        query.bindValue(1,inf.categoryName);
        query.bindValue(2,inf.color);
        query.bindValue(3,id().toString());
        query.exec();
    }
}

QString CDTImageLayer::path() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select path from imageLayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTImageLayer::absolutPath() const
{
    if (useRelativePath())
    {
        CDTProjectLayer *layer = qobject_cast<CDTProjectLayer *>(parent());
        QDir dir(layer->path());
        return dir.absoluteFilePath(path());
    }
    else
        return this->path();

}

bool CDTImageLayer::useRelativePath() const
{
    return useRelative;
}

int CDTImageLayer::bandCount() const
{
    QgsRasterLayer* layer = (QgsRasterLayer*)canvasLayer();
    if (layer==NULL) return 0;
    return layer->bandCount();
}

QList<CDTImageLayer *> CDTImageLayer::getLayers()
{
    return layers;
}

CDTImageLayer *CDTImageLayer::getLayer(const QUuid &id)
{
    foreach (CDTImageLayer *layer, layers) {
        if (id == layer->id())
            return layer;
    }
    return NULL;
}

void CDTImageLayer::setPath(QString path)
{
    if (this->path() == path)
        return;

    if (path.isEmpty())
        return;

    bool ret = false;
    QSqlQuery query(QSqlDatabase::database("category"));
    ret = query.prepare(QString("UPDATE %1 set path = ? where id =?").arg(tableName()));
    if (ret==false) return;
    query.bindValue(0,path);
    query.bindValue(1,this->id().toString());
    ret = query.exec();
    if (ret==false) return;

    emit pathChanged(path);
    emit layerChanged();
}

void CDTImageLayer::addExtraction()
{
    DialogNewExtraction *dlg = new DialogNewExtraction(this->id(),this->path(),this->fileSystem());
    if(dlg->exec()==DialogNewExtraction::Accepted)
    {
        CDTExtractionLayer *extraction = new CDTExtractionLayer(QUuid::createUuid(),this);
        extraction->initLayer(dlg->name(),dlg->fileID(),dlg->color(),dlg->borderColor(),dlg->opacity());
        extractionRoot->appendRow(extraction->standardKeyItem());
        addExtraction(extraction);
    }

    delete dlg;
}

void CDTImageLayer::addSegmentation()
{
    DialogNewSegmentation* dlg = new DialogNewSegmentation(this->id(),absolutPath(),this->fileSystem());
    if(dlg->exec()==DialogNewSegmentation::Accepted)
    {
        CDTSegmentationLayer *segmentation = new CDTSegmentationLayer(QUuid::createUuid(),this);
        segmentation->initLayer(
                    dlg->name(),dlg->shapefileID(),dlg->markfileID(),
                    dlg->method(),dlg->params(),/*dlg->databaseConnInfo()*/CDTDatabaseConnInfo(),dlg->borderColor());
        segmentationsRoot->appendRow(segmentation->standardKeyItem());
        addSegmentation(segmentation);
    }
    delete dlg;
}

void CDTImageLayer::remove()
{
    emit removeLayer(QList<QgsMapLayer*>()<<canvasLayer());
    emit removeImageLayer(this);
}

void CDTImageLayer::removeExtraction(CDTExtractionLayer *ext)
{    
    int index = extractions.indexOf(ext);
    if (index>=0)
    {
        QStandardItem* keyItem = ext->standardKeyItem();
        keyItem->parent()->removeRow(keyItem->index().row());
        extractions.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<ext->canvasLayer());
        fileSystem()->removeFile(ext->shapefileID());
        emit layerChanged();
    }
}

void CDTImageLayer::removeAllExtractionLayers()
{
    foreach (CDTExtractionLayer* ext, extractions) {        
        QStandardItem* keyItem = ext->standardKeyItem();
        keyItem->parent()->removeRow(keyItem->index().row());
        emit removeLayer(QList<QgsMapLayer*>()<<ext->canvasLayer());
        fileSystem()->removeFile(ext->shapefileID());
        delete ext;
    }
    extractions.clear();
    emit layerChanged();
}

void CDTImageLayer::removeSegmentation(CDTSegmentationLayer *sgmt)
{
    int index = segmentations.indexOf(sgmt);
    if (index>=0)
    {        
        QStandardItem* keyItem = sgmt->standardKeyItem();
        keyItem->parent()->removeRow(keyItem->index().row());
        segmentations.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<sgmt->canvasLayer());
        fileSystem()->removeFile(sgmt->shapefilePath());
        fileSystem()->removeFile(sgmt->markfilePath());
        delete sgmt;
        emit layerChanged();
    }
}

void CDTImageLayer::removeAllSegmentationLayers()
{
    foreach (CDTSegmentationLayer* sgmt, segmentations) {        
        QStandardItem* keyItem = sgmt->standardKeyItem();
        keyItem->parent()->removeRow(keyItem->index().row());
        emit removeLayer(QList<QgsMapLayer*>()<<sgmt->canvasLayer());
        fileSystem()->removeFile(sgmt->shapefilePath());
        fileSystem()->removeFile(sgmt->markfilePath());
        delete sgmt;
    }
    segmentations.clear();
    emit layerChanged();
}

void CDTImageLayer::setLayerOpacity(int opacity)
{
    QgsRasterLayer *rasterLayer =  qobject_cast<QgsRasterLayer*>(canvasLayer());
    if (rasterLayer)
    {
        rasterLayer->renderer()->setOpacity(opacity/100.);
        canvas()->refresh();
    }
}

void CDTImageLayer::setUseRelativePath(bool use)
{
    if (use==useRelative)
        return;

    CDTProjectLayer *layer = qobject_cast<CDTProjectLayer *>(parent());
    QDir dir(layer->path());

    if (use)//relative
    {
        QString relativePath = dir.relativeFilePath(this->path());
        this->setPath(relativePath);
    }
    else//absolute
    {
        QString absoluteFilePath = dir.absoluteFilePath(this->path());
        this->setPath(absoluteFilePath);
    }

    useRelative = QFileInfo(this->path()).isRelative();
}

void CDTImageLayer::redBandChanged(int bandIDFrom0)
{
    rBandID = bandIDFrom0 + 1;
    updateRenderer();
}

void CDTImageLayer::greenBandChanged(int bandIDFrom0)
{
    gBandID = bandIDFrom0 + 1;
    updateRenderer();
}

void CDTImageLayer::blueBandChanged(int bandIDFrom0)
{
    bBandID = bandIDFrom0 + 1;
    updateRenderer();
}

void CDTImageLayer::onEnhancementChanged(int enhancementStyle)
{
    this->enhancementStyle = (QgsContrastEnhancement::ContrastEnhancementAlgorithm)enhancementStyle;
    updateRenderer();
}

void CDTImageLayer::addExtraction(CDTExtractionLayer *extraction)
{
    extractions.push_back(extraction);
    emit layerChanged();
}


void CDTImageLayer::addSegmentation(CDTSegmentationLayer *segmentation)
{
    segmentations.push_back(segmentation);
    emit layerChanged();
}

void CDTImageLayer::updateRenderer()
{    
    QgsRasterLayer *layer = qobject_cast<QgsRasterLayer *>(this->canvasLayer());

    if (layer->bandCount()>=3)
    {
        //Band
        QgsRasterRenderer *renderer =
                new QgsMultiBandColorRenderer(layer->renderer(),rBandID,gBandID,bBandID);
        layer->setRenderer(renderer);
    }

    //Contrast
    layer->setContrastEnhancement(enhancementStyle,QgsRaster::ContrastEnhancementCumulativeCut,QgsRectangle(),0);
    MainWindow::getCurrentMapCanvas()->refresh();
}

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image)
{
    out<<image.id()<<image.path()<<image.name();

    out<<image.segmentations.size();
    for (int i=0;i<image.segmentations.size();++i)
        out<<*(image.segmentations[i]);

    out<<image.extractions.size();
    for (int i=0;i<image.extractions.size();++i)
        out<<*(image.extractions[i]);

    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    bool ret;
    ret = query.exec("select id,name,color from category where imageID = '" + image.id() + "'");
    if (!ret) qDebug()<<query.lastError();

    CDTCategoryInformationList categoryInfo;
    while(query.next())
    {
        categoryInfo.push_back(CategoryInformation
            (query.value(0).toString(),query.value(1).toString(),query.value(2).value<QColor>()));
    }
    out<<categoryInfo;


    query.exec(QString("select id,name,pointset_name from image_validation_samples where imageid = '%1'").arg(image.id()));
    QList<QVariantList> validationPoints;
    while(query.next())
    {
        QVariantList record;
        for (int i=0;i<query.record().count();++i)
            record<<query.value(i);
        validationPoints.push_back(record);
    }
    out<<validationPoints;

    foreach (QVariantList list, validationPoints) {
        QString validationID = list[0].toString();
        query.exec(QString("select id,categoryid from point_category where validationid = '%1'").arg(validationID));
        QMap<int,QString> point_category;
        while(query.next())
        {
            point_category.insert(query.value(0).toInt(),query.value(1).toString());
        }
        out<<point_category;
        qDebug()<<point_category;
    }
    return out ;
}


QDataStream &operator>>(QDataStream &in, CDTImageLayer &image)
{
    QUuid id;
    in>>id;
    image.setID(id);

    QString name,path;
    in>>path;
    in>>name;
    image.initLayer(name,path);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTSegmentationLayer* segmentation = new CDTSegmentationLayer(QUuid(),&image);
        in>>(*segmentation);
        image.segmentationsRoot->appendRow(segmentation->standardKeyItem());
        image.segmentations.push_back(segmentation);
    }

    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTExtractionLayer* extraction = new CDTExtractionLayer(QUuid(),&image);
        in>>(*extraction);
        image.extractionRoot->appendRow(extraction->standardKeyItem());
        image.extractions.push_back(extraction);
    }

    CDTCategoryInformationList info;
    in>>info;
    image.setCategoryInfo(info);

    QList<QVariantList> validationPoints;
    in>>validationPoints;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare(QString("insert into image_validation_samples values(?,?,?,?)"));
    foreach (QVariantList record, validationPoints) {
        query.bindValue(0,record[0]);
        query.bindValue(1,record[1]);
        query.bindValue(2,image.id().toString());
        query.bindValue(3,record[2]);
        query.exec();
    }

    for (int i=0;i<validationPoints.size();++i)
    {
        QMap<int,QString> point_category;
        in >>point_category;
        query.prepare(QString("insert into point_category values(?,?,?)"));
        foreach (int id, point_category.keys()) {
            query.bindValue(0,id);
            query.bindValue(1,point_category.value(id));
            query.bindValue(2,validationPoints[i][0].toString());
            query.exec();
        }
    }

    return in;
}

QDataStream &operator <<(QDataStream &out,const CategoryInformation &categoryInformation)
{
    out<<categoryInformation.id<<categoryInformation.categoryName<<categoryInformation.color;
    return out;
}

QDataStream &operator >>(QDataStream &in, CategoryInformation &categoryInformation)
{
    in>>categoryInformation.id>>categoryInformation.categoryName>>categoryInformation.color;
    return in;
}
