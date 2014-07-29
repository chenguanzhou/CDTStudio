#include "cdtclassificationlayer.h"
#include "cdtsegmentationlayer.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtvariantconverter.h"

CDTClassificationLayer::CDTClassificationLayer(QUuid uuid, QObject* parent)
    :CDTBaseLayer(uuid,parent),
      actionRemoveClassification(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove Classification"),this)),
      actionRename(new QAction(QIcon(":/Icon/Rename.png"),tr("Rename Classification"),this))
{
    keyItem   = new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION,CDTProjectTreeItem::EMPTY,QString(),this);
    valueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);

    paramRootItem = new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,CDTProjectTreeItem::EMPTY,tr("Method"),this);
    paramRootValueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(QList<QStandardItem*>()<<paramRootItem<<paramRootValueItem);

    normalizeItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(
                QList<QStandardItem*>()
                <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,tr("Normalize"),this)
                <<normalizeItem);
    pcaItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(
                QList<QStandardItem*>()
                <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,tr("PCA"),this)
                <<pcaItem);

    connect(this,SIGNAL(classificationLayerChanged()),this->parent(),SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(removeClassification(CDTClassificationLayer*)),this->parent(),SLOT(removeClassification(CDTClassificationLayer*)));
    connect(actionRemoveClassification,SIGNAL(triggered()),SLOT(remove()));
    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
}

CDTClassificationLayer::~CDTClassificationLayer()
{
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from classificationlayer where id = '"+uuid.toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
}

void CDTClassificationLayer::onContextMenuRequest(QWidget *parent)
{    
    QMenu *menu =new QMenu(parent);
    menu->addAction(actionRemoveClassification);
    menu->addSeparator();
    menu->addAction(actionRename);
    menu->exec(QCursor::pos());
}

void CDTClassificationLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input Classification Name"),
                tr("Segmentation rename:"), QLineEdit::Normal,
                this->name(), &ok);
    if (ok && !text.isEmpty())
        setName(text);
}

void CDTClassificationLayer::remove()
{
    emit removeClassification(this);
}

QString CDTClassificationLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTClassificationLayer::method() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select method from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QVariantMap CDTClassificationLayer::params() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select params from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();

    return variantToData<QVariantMap>(query.value(0));
}

QVariantList CDTClassificationLayer::data() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select data from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();

    return variantToData<QVariantList>(query.value(0));
}

QVariantMap CDTClassificationLayer::clsInfo() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select clsinfo from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return variantToData<QVariantMap>(query.value(0));
}

QString CDTClassificationLayer::normalizeMethod() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select normalizeMethod from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTClassificationLayer::pcaParams() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select pca from classificationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QgsFeatureRendererV2 *CDTClassificationLayer::renderer()
{
    QMap<QString,QVariant> clsInfo = this->clsInfo();

    QVariantList data = this->data();
    QMap<int,QColor> colorList;
    QSqlQuery query(QSqlDatabase::database("category"));

    foreach (QString categoryID, clsInfo.keys()) {
        query.exec(QString("select color from category where id = '%1'").arg(categoryID));
        query.next();
        QColor clr = query.value(0).value<QColor>();
        colorList.insert(clsInfo.value(categoryID).toInt(),clr);
    }

    QgsCategoryList categoryList;
    for(int i=0;i<data.size();++i)
    {
        QgsSimpleFillSymbolLayerV2* symbolLayer = new QgsSimpleFillSymbolLayerV2();
        symbolLayer->setColor(colorList.value(data[i].toInt()));
        QgsRendererCategoryV2 rend;
        rend.setValue(i);
        rend.setSymbol(new QgsFillSymbolV2(QgsSymbolLayerV2List()<<symbolLayer));
        rend.setLabel(QString::number(i));
        categoryList<<rend;
    }

    QgsCategorizedSymbolRendererV2* categorizedSymbolRenderer = new QgsCategorizedSymbolRendererV2("GridCode",categoryList);
    return categorizedSymbolRenderer;
}

void CDTClassificationLayer::setName(const QString &name)
{
    if (this->name()==name)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE classificationlayer set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
}

void CDTClassificationLayer::initClassificationLayer(
        const QString &name,
        const QString &methodName,
        const QMap<QString, QVariant> &params,
        const QList<QVariant> &data,
        const QMap<QString, QVariant> &clsInfo,
        const QString &normalizeMethod,
        const QString &pcaParams)
{
    keyItem->setText(name);

    paramRootItem->removeRows(0,paramRootItem->rowCount());
    paramRootValueItem->setText(methodName);
    QStringList keys = params.keys();
    foreach (QString key, keys) {
        QVariant value = params.value(key);
        paramRootItem->appendRow(
                    QList<QStandardItem*>()
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,key,this)
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,value.toString(),this)
                    );
    }

    normalizeItem->setText(normalizeMethod);
    pcaItem->setText(pcaParams);

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("insert into classificationlayer VALUES(?,?,?,?,?,?,?,?,?)");
    query.addBindValue(id().toString());
    query.addBindValue(name);
    query.addBindValue(methodName);
    query.addBindValue(dataToVariant(params));
    query.addBindValue(dataToVariant(data));
    query.addBindValue(dataToVariant(clsInfo));
    query.addBindValue(normalizeMethod);
    query.addBindValue(pcaParams);
    query.addBindValue(((CDTSegmentationLayer*)parent())->id().toString());
    query.exec();

    //dynamic properties
    foreach (QString key, params.keys()) {
        this->setProperty((QString("   ")+key).toLocal8Bit().constData(),params.value(key.toLocal8Bit().constData()));
    }
}

QDataStream &operator<<(QDataStream &out, const CDTClassificationLayer &classification)
{
    out<<classification.uuid
      <<classification.name()
     <<classification.method()
    <<classification.params()
    <<classification.data()
    <<classification.clsInfo()
    <<classification.normalizeMethod()
    <<classification.pcaParams();
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTClassificationLayer &classification)
{
    QString name;
    QString method;
    QMap<QString, QVariant> param;
    QList<QVariant> data;
    QMap<QString, QVariant> clsInfo;
    QString normalize;
    QString pca;

    in>>classification.uuid>>name>>method>>param>>data>>clsInfo>>normalize>>pca;
    classification.initClassificationLayer(name,method,param,data,clsInfo,normalize,pca);

    return in;
}



