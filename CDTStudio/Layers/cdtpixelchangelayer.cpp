#include "cdtpixelchangelayer.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtvariantconverter.h"
#include "cdtprojecttreeitem.h"


QMap<QString,QMetaObject> CDTPixelChangeLayer::changeLayerMetaObjects;
CDTPixelChangeLayer::CDTPixelChangeLayer(QUuid uuid, QObject *parent) :
    CDTBaseLayer(uuid,parent)
{
    CDTProjectTreeItem *keyItem
            = new CDTProjectTreeItem(CDTProjectTreeItem::PIXELCHANGE,CDTProjectTreeItem::RASTER,QString(),this);
    setKeyItem(keyItem);
}

CDTPixelChangeLayer::~CDTPixelChangeLayer()
{

}

QString CDTPixelChangeLayer::image_t1() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t1 from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTPixelChangeLayer::image_t2() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t2 from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QVariantMap CDTPixelChangeLayer::params() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select params from pbcd_binary where id ='" + this->id().toString() +"'");
    query.next();

    return variantToData<QVariantMap>(query.value(0));
}

QStringList CDTPixelChangeLayer::files() const
{
    return QStringList();
}

void CDTPixelChangeLayer::initLayer(const QString &name, const QString &image_t1, const QString &image_t2, const QVariantMap &params)
{
    standardKeyItem()->setText(name);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into pbcd_binary VALUES(?,?,?,?,?)");
    if (ret==false)
    {
        logger()->error("Init CDTPixelChangeLayer Fialed!");
        return;
    }
    query.bindValue(0,id().toString());
    query.bindValue(1,name);
    query.bindValue(2,image_t1);
    query.bindValue(3,image_t2);
    query.bindValue(4,dataToVariant(params));
    query.exec();
}


QDataStream &operator<<(QDataStream &out, const CDTPixelChangeLayer &layer)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from pbcd_binary where id ='" + layer.id().toString() +"'");
    query.next();
    out <<layer.id()                 //id
        <<layer.type               //type
        <<query.value(1).toString() //name
        <<query.value(2).toString() //image_t1
        <<query.value(3).toString() //image_t2
        <<query.value(4);           //params
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTPixelChangeLayer &layer)
{
    QUuid id;
    int type;
    in>>id>>type;

    layer.setID(id);
    layer.type = static_cast<CDTPixelChangeLayer::ChangeType>(type);

    QString name,image_t1,image_t2;
    QVariant params;
    in>>name>>image_t1>>image_t2>>params;

    layer.initLayer(name,image_t1,image_t2,variantToData<QVariantMap>(params));
    return in;
}
