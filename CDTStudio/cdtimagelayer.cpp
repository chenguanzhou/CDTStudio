#include "cdtimagelayer.h"
#include <QMenu>
#include <QInputDialog>
#include "dialognewsegmentation.h"
#include "cdtproject.h"
#include <qgsrasterlayer.h>
#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
#include <QMessageBox>

QList<CDTImageLayer *> CDTImageLayer::layers;

CDTImageLayer::CDTImageLayer(QUuid uuid, QObject *parent)
    : CDTBaseObject(uuid,parent),
      addSegmentationLayer(new QAction(QIcon(":/Icon/add.png"),tr("Add Segmentation"),this)),
      removeImage(new QAction(QIcon(":/Icon/remove.png"),tr("Remove Image"),this)),
      removeAllSegmentations(new QAction(QIcon(":/Icon/remove.png"),tr("Remove All Segmentations"),this)),
      actionRename(new QAction(QIcon(":/Icon/rename.png"),tr("Rename Image"),this)),
      actionCategoryInformation(new QAction(tr("Category Information"),this)),
      trainingForm(NULL)
{
    keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::RASTER,QString(),this);
    valueItem=new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    segmentationsroot = new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,CDTProjectTreeItem::GROUP,tr("segmentations"),this);
    keyItem->appendRow(segmentationsroot);

    layers.push_back(this);

    connect(addSegmentationLayer,SIGNAL(triggered()),this,SLOT(addSegmentation()));
    connect(removeImage,SIGNAL(triggered()),this,SLOT(remove()));
    connect(this,SIGNAL(removeImageLayer(CDTImageLayer*)),(CDTProject*)(this->parent()),SLOT(removeImageLayer(CDTImageLayer*)));
    connect(this,SIGNAL(imageLayerChanged()),(CDTProject*)(this->parent()),SIGNAL(projectChanged()));
    connect(removeAllSegmentations,SIGNAL(triggered()),this,SLOT(removeAllSegmentationLayers()));
    connect(actionRename,SIGNAL(triggered()),this,SLOT(rename()));
    connect(actionCategoryInformation,SIGNAL(triggered()),this,SLOT(onActionCategoryInformation()));
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
    query.bindValue(3,((CDTProject*)parent())->id().toString());
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

void CDTImageLayer::addSegmentation(CDTSegmentationLayer *segmentation)
{
    segmentations.push_back(segmentation);
    emit imageLayerChanged();
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

void CDTImageLayer::addSegmentation()
{
    DialogNewSegmentation* dlg = new DialogNewSegmentation(this->path());
    if(dlg->exec()==DialogNewSegmentation::Accepted)
    {
        CDTSegmentationLayer *segmentation = new CDTSegmentationLayer(QUuid::createUuid(),this->path(),this);
        segmentation->setLayerInfo(dlg->name(),dlg->shapefilePath(),dlg->markfilePath());
        segmentation->setMethodParams(dlg->method(),dlg->params());
        segmentationsroot->appendRow(segmentation->standardItems());
        addSegmentation(segmentation);
    }
    delete dlg;
}

void CDTImageLayer::remove()
{
    emit removeLayer(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit removeImageLayer(this);
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
        delete sgmt;
        emit imageLayerChanged();
    }
}

void CDTImageLayer::removeAllSegmentationLayers()
{
    foreach (CDTSegmentationLayer* sgmt, segmentations) {
        QStandardItem* keyItem = sgmt->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        emit removeLayer(QList<QgsMapLayer*>()<<sgmt->canvasLayer());
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

void CDTImageLayer::onActionCategoryInformation()
{

}

void CDTImageLayer::onContextMenuRequest(QWidget *parent)
{    
    QMenu *menu =new QMenu(parent);

    menu->addAction(addSegmentationLayer);
    menu->addAction(removeImage);
    menu->addAction(removeAllSegmentations);
    menu->addSeparator();
    menu->addAction(actionRename);
    menu->addSeparator();
    menu->addAction(actionCategoryInformation);

    menu->exec(QCursor::pos());
}

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image)
{
    out<<image.uuid<<image.path()<<image.name()<<image.segmentations.size();
    for (int i=0;i<image.segmentations.size();++i)
        out<<*(image.segmentations[i]);

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
        CDTSegmentationLayer* segmentation = new CDTSegmentationLayer(QUuid(),image.path(),&image);
        in>>(*segmentation);
        image.segmentationsroot->appendRow(segmentation->standardItems());
        image.segmentations.push_back(segmentation);
    }
    CDTCategoryInformationList info;
    in>>info;
    image.setCategoryInfo(info);
    return in;
}
