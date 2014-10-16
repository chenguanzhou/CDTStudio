#include "cdtchangelayer.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtvariantconverter.h"
#include "cdtprojecttreeitem.h"


QMap<QString,QMetaObject> CDTChangeLayer::changeLayerMetaObjects;
CDTChangeLayer::CDTChangeLayer(QUuid uuid, QObject *parent) :
    CDTBaseLayer(uuid,parent)
{
    keyItem = new CDTProjectTreeItem(CDTProjectTreeItem::CHANGE,CDTProjectTreeItem::RASTER,QString(),this);

}

CDTChangeLayer::~CDTChangeLayer()
{

}

QString CDTChangeLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from changes where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTChangeLayer::image_t1() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t1 from changes where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTChangeLayer::image_t2() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select image_t2 from changes where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QVariantMap CDTChangeLayer::params() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select params from changes where id ='" + this->id().toString() +"'");
    query.next();

    return variantToData<QVariantMap>(query.value(0));
}

QStringList CDTChangeLayer::files() const
{
    return QStringList();
}

void CDTChangeLayer::setName(const QString &name)
{
    if (this->name() == name)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE changes set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    //    emit nameChanged();
}

void CDTChangeLayer::initLayer(const QString &name, const QString &image_t1, const QString &image_t2, const QVariantMap &params)
{
    keyItem->setText(name);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into changes VALUES(?,?,?,?,?)");
    query.bindValue(0,uuid.toString());
    query.bindValue(1,name);
    query.bindValue(2,image_t1);
    query.bindValue(3,image_t2);
    query.bindValue(4,dataToVariant(params));
    query.exec();
}


QDataStream &operator<<(QDataStream &out, const CDTChangeLayer &layer)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from changes where id ='" + layer.id().toString() +"'");
    query.next();
    out <<layer.id()                 //id
        <<layer.type               //type
        <<query.value(1).toString() //name
        <<query.value(2).toString() //image_t1
        <<query.value(3).toString() //image_t2
        <<query.value(4);           //params
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTChangeLayer &layer)
{
    int type;
    in>>layer.uuid>>type;
    layer.type = static_cast<CDTChangeLayer::ChangeType>(type);

    QString name,image_t1,image_t2;
    QVariant params;
    in>>name>>image_t1>>image_t2>>params;

    layer.initLayer(name,image_t1,image_t2,variantToData<QVariantMap>(params));
    return in;
}
