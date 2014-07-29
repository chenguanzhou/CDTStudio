#include "cdtimagelayer.h"
#include "stable.h"
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
      actionAddExtractionLayer(new QAction(QIcon(":/Icon/Add.png"),tr("Add Extraction"),this)),
      actionAddSegmentationLayer(new QAction(QIcon(":/Icon/Add.png"),tr("Add Segmentation"),this)),
      actionRemoveImage(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove Image"),this)),
      actionRemoveAllExtractions(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove All Extractions"),this)),
      actionRemoveAllSegmentations(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove All Segmentations"),this)),
      actionRename(new QAction(QIcon(":/Icon/Rename.png"),tr("Rename Image"),this))
{
    keyItem = new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::RASTER,QString(),this);
    valueItem
            = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    extractionRoot
            = new CDTProjectTreeItem(CDTProjectTreeItem::EXTRACTION_ROOT,CDTProjectTreeItem::GROUP,tr("extractions"),this);
    segmentationsRoot
            = new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,CDTProjectTreeItem::GROUP,tr("segmentations"),this);
    keyItem->appendRow(extractionRoot);
    keyItem->appendRow(segmentationsRoot);

    layers.push_back(this);


    connect(this,SIGNAL(removeImageLayer(CDTImageLayer*)),(CDTProjectLayer*)(this->parent()),SLOT(removeImageLayer(CDTImageLayer*)));
    connect(this,SIGNAL(imageLayerChanged()),(CDTProjectLayer*)(this->parent()),SIGNAL(projectChanged()));

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
    ret = query.exec("delete from imagelayer where id = '"+uuid.toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
    layers.removeAll(this);
}

void CDTImageLayer::setName(const QString &name)
{
    if (this->name() == name)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE imageLayer set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit imageLayerChanged();
}

void CDTImageLayer::setNameAndPath(const QString &name, const QString &path)
{
    QgsRasterLayer *newCanvasLayer = new QgsRasterLayer(path,QFileInfo(path).completeBaseName());
    if (!newCanvasLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open image ")+path+tr(" failed!"));
        delete newCanvasLayer;
        return;
    }

    keyItem->setText(name);
    valueItem->setText(path);
    if (mapCanvasLayer)
        delete mapCanvasLayer;
    mapCanvasLayer = newCanvasLayer;

    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer,TRUE);
    keyItem->setMapLayer(mapCanvasLayer);

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
    query.bindValue(0,uuid.toString());
    query.bindValue(1,name);
    query.bindValue(2,path);
    query.bindValue(3,((CDTProjectLayer*)parent())->id().toString());
    query.exec();

    emit appendLayers(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit imageLayerChanged();
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
        query.bindValue(3,uuid.toString());
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

QString CDTImageLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from imageLayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

int CDTImageLayer::bandCount() const
{
    QgsRasterLayer* layer = (QgsRasterLayer*)mapCanvasLayer;
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
        if (id == layer->uuid)
            return layer;
    }
    return NULL;
}

void CDTImageLayer::addExtraction()
{
    DialogNewExtraction *dlg = new DialogNewExtraction(this->path(),this->fileSystem());
    if(dlg->exec()==DialogNewExtraction::Accepted)
    {
        CDTExtractionLayer *extraction = new CDTExtractionLayer(QUuid::createUuid(),this);
        extraction->initLayer(dlg->name(),dlg->fileID(),dlg->color(),dlg->borderColor(),dlg->opacity());
        extractionRoot->appendRow(extraction->standardItems());
        addExtraction(extraction);
    }

    delete dlg;
}

void CDTImageLayer::addSegmentation()
{
    DialogNewSegmentation* dlg = new DialogNewSegmentation(this->path(),this->fileSystem());
    if(dlg->exec()==DialogNewSegmentation::Accepted)
    {
        CDTSegmentationLayer *segmentation = new CDTSegmentationLayer(QUuid::createUuid(),this);
        segmentation->initSegmentationLayer(
                    dlg->name(),dlg->shapefileID(),dlg->markfileID(),
                    dlg->method(),dlg->params(),dlg->databaseConnInfo(),dlg->borderColor());
        segmentationsRoot->appendRow(segmentation->standardItems());
        addSegmentation(segmentation);
    }
    delete dlg;
}

void CDTImageLayer::remove()
{
    emit removeLayer(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit removeImageLayer(this);
}

void CDTImageLayer::removeExtraction(CDTExtractionLayer *ext)
{    
    int index = extractions.indexOf(ext);
    if (index>=0)
    {
        QStandardItem* keyItem = ext->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        extractions.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<ext->canvasLayer());
        fileSystem()->removeFile(ext->shapefileID());
//        delete ext;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        emit imageLayerChanged();
    }
}

void CDTImageLayer::removeAllExtractionLayers()
{
    foreach (CDTExtractionLayer* ext, extractions) {        
        QStandardItem* keyItem = ext->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        emit removeLayer(QList<QgsMapLayer*>()<<ext->canvasLayer());
        fileSystem()->removeFile(ext->shapefileID());
        delete ext;
    }
    extractions.clear();
    emit imageLayerChanged();
}

void CDTImageLayer::removeSegmentation(CDTSegmentationLayer *sgmt)
{
    int index = segmentations.indexOf(sgmt);
    if (index>=0)
    {        
        QStandardItem* keyItem = sgmt->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        segmentations.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<sgmt->canvasLayer());
        fileSystem()->removeFile(sgmt->shapefilePath());
        fileSystem()->removeFile(sgmt->markfilePath());
//        delete sgmt; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        emit imageLayerChanged();
    }
}

void CDTImageLayer::removeAllSegmentationLayers()
{
    foreach (CDTSegmentationLayer* sgmt, segmentations) {        
        QStandardItem* keyItem = sgmt->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        emit removeLayer(QList<QgsMapLayer*>()<<sgmt->canvasLayer());
        fileSystem()->removeFile(sgmt->shapefilePath());
        fileSystem()->removeFile(sgmt->markfilePath());
        delete sgmt;
    }
    segmentations.clear();
    emit imageLayerChanged();
}

void CDTImageLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Input Image Name"),
                                         tr("Image rename:"), QLineEdit::Normal,
                                         this->name(), &ok);
    if (ok && !text.isEmpty())
    {
        setName(text);
    }
}

void CDTImageLayer::onContextMenuRequest(QWidget *parent)
{
    QMenu *menu =new QMenu(parent);

    menu->addAction(actionRename);
    menu->addAction(actionRemoveImage);
    menu->addSeparator();
    menu->addAction(actionAddExtractionLayer);
    menu->addAction(actionRemoveAllExtractions);
    menu->addSeparator();
    menu->addAction(actionAddSegmentationLayer);
    menu->addAction(actionRemoveAllSegmentations);

    menu->exec(QCursor::pos());
}

void CDTImageLayer::addExtraction(CDTExtractionLayer *extraction)
{
    extractions.push_back(extraction);
    emit imageLayerChanged();
}


void CDTImageLayer::addSegmentation(CDTSegmentationLayer *segmentation)
{
    segmentations.push_back(segmentation);
    emit imageLayerChanged();
}

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image)
{
    out<<image.uuid<<image.path()<<image.name();

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
    return out ;
}


QDataStream &operator>>(QDataStream &in, CDTImageLayer &image)
{
    in>>image.uuid;
    QString name,path;
    in>>path;
    in>>name;
    image.setNameAndPath(name,path);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTSegmentationLayer* segmentation = new CDTSegmentationLayer(QUuid(),&image);
        in>>(*segmentation);
        image.segmentationsRoot->appendRow(segmentation->standardItems());
        image.segmentations.push_back(segmentation);
    }

    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTExtractionLayer* extraction = new CDTExtractionLayer(QUuid(),&image);
        in>>(*extraction);
        image.extractionRoot->appendRow(extraction->standardItems());
        image.extractions.push_back(extraction);
    }

    CDTCategoryInformationList info;
    in>>info;
    image.setCategoryInfo(info);
    return in;
}
