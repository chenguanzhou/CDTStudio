#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include "stable.h"
#include <qgsvectorlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsfillsymbollayerv2.h>
#include <qgscategorizedsymbolrendererv2.h>
#include "cdtvariantconverter.h"

CDTClassification::CDTClassification(QUuid uuid, QObject* parent)
    :CDTBaseObject(uuid,parent),
      actionRemoveClassification(new QAction(tr("Remove Classification"),this))
{
    keyItem   = new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION,CDTProjectTreeItem::VECTOR,QString(),this);
    valueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);

    paramRootItem = new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,CDTProjectTreeItem::EMPTY,tr("Method"),this);
    paramRootValueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(QList<QStandardItem*>()<<paramRootItem<<paramRootValueItem);

    connect(this,SIGNAL(classificationLayerChanged()),this->parent(),SIGNAL(segmentationChanged()));
    connect(actionRemoveClassification,SIGNAL(triggered()),this,SLOT(remove()));
    connect(this,SIGNAL(removeClassification(CDTClassification*)),this->parent(),SLOT(removeClassification(CDTClassification*)));
}

void CDTClassification::onContextMenuRequest(QWidget *parent)
{
    actionRemoveClassification->setIcon(QIcon(":/Icon/remove.png"));
    QMenu *menu =new QMenu(parent);

    menu->addAction(actionRemoveClassification);
    menu->exec(QCursor::pos());
}

void CDTClassification::remove()
{
    emit removeClassification(this);
}

QString CDTClassification::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTClassification::method() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select method from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QVariantMap CDTClassification::params() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select params from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();

    return variantToData<QVariantMap>(query.value(0));
}

QVariantList CDTClassification::data() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select data from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();

    return variantToData<QVariantList>(query.value(0));
}

QVariantMap CDTClassification::clsInfo() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select clsinfo from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return variantToData<QVariantMap>(query.value(0));
}

QgsFeatureRendererV2 *CDTClassification::renderer()
{
    QList<QVariant> data = this->data();
    QMap<QString,QVariant> clsInfo = this->clsInfo();
    QMap<QString,QgsFillSymbolV2*> symbols;
    qDebug()<<"data.size():"<<data.size();
    qDebug()<<"clsInfo.size():"<<clsInfo.size();


    foreach (QString categoryID, clsInfo.keys()) {
        QSqlQuery query(QSqlDatabase::database("category"));
        query.exec(QString("select color from category where id = '%1'").arg(categoryID));
        query.next();
        QColor clr = query.value(0).value<QColor>();
        QgsSimpleFillSymbolLayerV2* symbolLayer = new QgsSimpleFillSymbolLayerV2();
        symbolLayer->setColor(clr);
        QgsFillSymbolV2 *fillSymbol = new QgsFillSymbolV2(QgsSymbolLayerV2List()<<symbolLayer);
        symbols.insert(categoryID,fillSymbol);
    }

    qDebug()<<"symbols.size():"<<symbols.size();

    QgsCategoryList categoryList;
    for (int objID = 0;objID<data.size();++objID)
    {
        QString categoryID = data[objID].toString();
//        categoryList<<QgsRendererCategoryV2(objID,symbols[categoryID],QString::number(objID));
        QgsRendererCategoryV2 rend;
        rend.setValue(objID);
        rend.setSymbol(symbols[categoryID]);
        rend.setLabel(QString::number(objID));
        categoryList<<rend;
    }
    QgsCategorizedSymbolRendererV2* categorizedSymbolRenderer = new QgsCategorizedSymbolRendererV2("GridCode",categoryList);
    qDebug()<<"hehe";
    return categorizedSymbolRenderer;
}

void CDTClassification::setName(const QString &name)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE classificationlayer set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
}

//void CDTClassification::setMethod(const QString &methodName)
//{
//    QSqlQuery query(QSqlDatabase::database("category"));
//    query.prepare("UPDATE classificationlayer set method = ? where id =?");
//    query.bindValue(0,QVariant(methodName));
//    query.bindValue(1,this->id().toString());
//    query.exec();
//}

//void CDTClassification::setParam(const QMap<QString, QVariant> &param)
//{
//    QSqlQuery query(QSqlDatabase::database("category"));
//    query.prepare("UPDATE classificationlayer set params = ? where id =?");
//    query.bindValue(0,QVariant(param));
//    query.bindValue(1,this->id().toString());
//    query.exec();
//}

//void CDTClassification::setData(const QList<QVariant> &data)
//{
//    QSqlQuery query(QSqlDatabase::database("category"));
//    query.prepare("UPDATE classificationlayer set data = ? where id =?");
//    query.bindValue(0,QVariant(data));
//    query.bindValue(1,this->id().toString());
//    query.exec();
//}

//void CDTClassification::setClsInfo(const QMap<QString, QVariant> &clsInfo) const
//{
//    QSqlQuery query(QSqlDatabase::database("category"));
//    query.prepare("UPDATE classificationlayer set clsinfo = ? where id =?");
//    query.bindValue(0,QVariant(clsInfo));
//    query.bindValue(1,this->id().toString());
//    query.exec();
//}

void CDTClassification::initClassificationLayer(
        const QString &name,
        const QString &methodName,
        const QMap<QString, QVariant> &param,
        const QList<QVariant> &data,
        const QMap<QString, QVariant> &clsInfo  )
{
    keyItem->setText(name);

    paramRootItem->removeRows(0,paramRootItem->rowCount());
    paramRootValueItem->setText(methodName);
    QStringList keys = param.keys();
    foreach (QString key, keys) {
        QVariant value = param.value(key);
        paramRootItem->appendRow(
                    QList<QStandardItem*>()
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,key,this)
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,value.toString(),this)
                    );
    }

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("insert into classificationlayer VALUES(?,?,?,?,?,?,?)");
    query.addBindValue(id().toString());
    query.addBindValue(name);
    query.addBindValue(methodName);
    query.addBindValue(dataToVariant(param));
    query.addBindValue(dataToVariant(data));
    query.addBindValue(dataToVariant(clsInfo));
    query.addBindValue(((CDTSegmentationLayer*)parent())->id().toString());
    query.exec();

    keyItem->setText(name);

    qDebug()<<QVariant::fromValue(clsInfo);
    qDebug()<<this->clsInfo();
}

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification)
{
    out<<classification.uuid
      <<classification.name()
     <<classification.method()
    <<classification.params()
    <<classification.data()
    <<classification.clsInfo();
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTClassification &classification)
{
    QString name;
    QString method;
    QMap<QString, QVariant> param;
    QList<QVariant> data;
    QMap<QString, QVariant> clsInfo;

    in>>classification.uuid>>name>>method>>param>>data>>clsInfo;
    classification.initClassificationLayer(name,method,param,data,clsInfo);

    return in;
}



