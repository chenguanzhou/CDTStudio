#include "cdtpbcdlayer.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtvariantconverter.h"
#include "cdtprojecttreeitem.h"


CDTPBCDLayer::CDTPBCDLayer(QUuid uuid, QObject *parent) :
    CDTBaseLayer(uuid,parent)
{
    keyItem = new CDTProjectTreeItem(CDTProjectTreeItem::CHANGE,CDTProjectTreeItem::RASTER,QString(),this);

}

CDTPBCDLayer::~CDTPBCDLayer()
{

}

QString CDTPBCDLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t1 from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTPBCDLayer::diffImage() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select diff_image from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTPBCDLayer::path_t1() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t1 from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();
    QString tempDiffPath;
    this->fileSystem()->getFile(query.value(0).toString(),tempDiffPath);
    return tempDiffPath;
}

QString CDTPBCDLayer::path_t2() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t2 from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();
    QString tempDiffPath;
    this->fileSystem()->getFile(query.value(0).toString(),tempDiffPath);
    return tempDiffPath;
}

void CDTPBCDLayer::setName(const QString &name)
{
    if (this->name() == name)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE pbcd_binary set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
//    emit nameChanged();
}

void CDTPBCDLayer::initPBCDBinaryLayer(const QString &name, const QString &diff_image, const QString &image_t1, const QString &image_t2, const QVariantMap &params)
{
    QString tempDiffPath;
    this->fileSystem()->getFile(diff_image,tempDiffPath);
    QgsRasterLayer *newLayer = new QgsRasterLayer(tempDiffPath);
    if (!newLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open diff image ")+tempDiffPath+tr(" failed!"));
        delete newLayer;
        return;
    }

    if (mapCanvasLayer)
    {
        QgsMapLayerRegistry::instance()->removeMapLayer(mapCanvasLayer->id());
        delete mapCanvasLayer;
    }
    mapCanvasLayer = newLayer;

    keyItem->setText(name);

    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer);
    keyItem->setMapLayer(mapCanvasLayer);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into pbcd_binary VALUES(?,?,?,?,?,?)");
    query.bindValue(0,uuid.toString());
    query.bindValue(1,name);
    query.bindValue(2,diff_image);
    query.bindValue(3,image_t1);
    query.bindValue(4,image_t2);
    query.bindValue(5,dataToVariant(params));
    query.exec();
}
