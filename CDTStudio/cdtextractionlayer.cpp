#include "cdtextractionlayer.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"

CDTExtractionLayer::CDTExtractionLayer(QUuid uuid, QObject *parent) :
    CDTBaseObject(uuid,parent)
{
    keyItem   = new CDTProjectTreeItem(CDTProjectTreeItem::EXTRACTION,CDTProjectTreeItem::VECTOR,QString(),this);
    valueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    connect(this,SIGNAL(nameChanged()),this,SIGNAL(extractionChanged()));
}

QString CDTExtractionLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTExtractionLayer::shapefilePath() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select shapefilePath from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QColor CDTExtractionLayer::color() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select color from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
    qDebug()<<query.value(0);
    return query.value(0).value<QColor>();
}

QString CDTExtractionLayer::imagePath() const
{
    return ((CDTImageLayer*)parent())->path();
}

void CDTExtractionLayer::onContextMenuRequest(QWidget *parent)
{

}

void CDTExtractionLayer::setName(const QString &name)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE extractionlayer set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit nameChanged();
}

void CDTExtractionLayer::setBorderColor(const QColor &clr)
{
//    QSqlQuery query(QSqlDatabase::database("category"));
//    query.prepare("UPDATE extractionlayer set color = ? where id =?");
//    query.bindValue(0,clr);
//    query.bindValue(1,this->id().toString());
//    query.exec();

//    setOriginRenderer();
//    this->mapCanvas->refresh();
//    emit nameChanged();
}

void CDTExtractionLayer::initLayer(const QString &name, const QString &shpPath, const QColor &color)
{
    QString tempShpPath;
    this->fileSystem()->getFile(shpPath,tempShpPath);
    QgsVectorLayer *newLayer = new QgsVectorLayer(tempShpPath,QFileInfo(tempShpPath).completeBaseName(),"ogr");
    if (!newLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open shapefile ")+tempShpPath+tr(" failed!"));
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
    ret = query.prepare("insert into extractionlayer VALUES(?,?,?,?,?)");
    qDebug()<<ret;
    query.addBindValue(uuid.toString());
    query.addBindValue(name);
    query.addBindValue(shpPath);
    query.addBindValue(color);
    query.addBindValue(((CDTImageLayer*)parent())->id().toString());
    ret = query.exec();
    qDebug()<<ret;
    emit appendLayers(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit extractionChanged();
}

QDataStream &operator<<(QDataStream &out, const CDTExtractionLayer &extraction)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from extractionlayer where id ='" + extraction.id().toString() +"'");
    query.next();

    out<<extraction.id()            //id
      <<query.value(1).toString()   //name
     <<query.value(2).toString()    //shapefile
    <<query.value(3);               //color

    return out;
}


QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction)
{
    in>>extraction.uuid;
    QString name,shp;
    in>>name>>shp;
    QVariant temp;
    in>>temp;
    QColor color = temp.value<QColor>();

    extraction.initLayer(name,shp,color);
    return in;
}
