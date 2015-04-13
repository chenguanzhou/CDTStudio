#include "cdtclassificationlayer.h"

#include "stable.h"
#include "mainwindow.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassifierassessmentform.h"
#include "cdtprojecttreeitem.h"
#include "cdtvariantconverter.h"
#include "cdtexportclassificationlayerhelper.h"


QList<CDTClassificationLayer *> CDTClassificationLayer::layers;
CDTClassificationLayer::CDTClassificationLayer(QUuid uuid, QObject* parent)
    :CDTBaseLayer(uuid,parent)
{
    layers.push_back(this);

    setKeyItem(new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION,CDTProjectTreeItem::EMPTY,QString(),this));

    //actions
    QAction *actionRename =
            new QAction(QIcon(":/Icons/Rename.png"),tr("Rename Classification"),this);
    QAction *actionExportClsLayer =
            new QAction(QIcon(":/Icons/Export.png"),tr("Export Classification Layer"),this);
    QAction *actionAccuracyAssessment =
            new QAction(tr("Accuracy Assessment"),this);
    QAction* actionRemoveClassification =
            new QAction(QIcon(":/Icons/Remove.png"),tr("Remove Classification"),this);

    setActions(QList<QList<QAction *> >()
               <<(QList<QAction*>()<</*actionSetLayerTransparency<<*/actionRename<<actionExportClsLayer<<actionAccuracyAssessment)
               <<(QList<QAction*>()<<actionRemoveClassification));



    connect(this,SIGNAL(removeClassification(CDTClassificationLayer*)),this->parent(),SLOT(removeClassification(CDTClassificationLayer*)));
    connect(actionRemoveClassification,SIGNAL(triggered()),SLOT(remove()));
    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
    connect(actionExportClsLayer,SIGNAL(triggered()),SLOT(exportLayer()));
    connect(actionAccuracyAssessment,SIGNAL(triggered()),SLOT(showAccuracy()));
}

CDTClassificationLayer::~CDTClassificationLayer()
{
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from classificationlayer where id = '"+id().toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    layers.removeAll(this);
}

void CDTClassificationLayer::remove()
{
    emit removeClassification(this);
}

void CDTClassificationLayer::exportLayer()
{
    QString shpPath= QFileDialog::getSaveFileName(NULL,tr("Export"),QString(), "ESRI Shapefile(*.shp)");
    if(shpPath.isEmpty())
        return;

    if (CDTExportClassificationLayerHelper::exportClassification(id(),tr("Category"),shpPath))
        QMessageBox::information(NULL,tr("Completed"),tr("Export file completed!"));
    else
        QMessageBox::critical(NULL,tr("Error"),tr("Export classification layer failed!"));
}

void CDTClassificationLayer::showAccuracy()
{    
    QDialog dlg;
    dlg.setWindowTitle(tr("Accuracy Assessment"));
    QVBoxLayout *vbox = new QVBoxLayout(&dlg);
    vbox->setMargin(0);
    dlg.setLayout(vbox);
    CDTClassifierAssessmentForm *form = new CDTClassifierAssessmentForm(&dlg);
    vbox->addWidget(form);

    form->setClassification(id());
    dlg.exec();
}

//QString CDTClassificationLayer::name() const
//{
//    QSqlDatabase db = QSqlDatabase::database("category");
//    QSqlQuery query(db);
//    query.exec("select name from classificationlayer where id ='" + this->id().toString() +"'");
//    query.next();
//    return query.value(0).toString();
//}

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

QStringList CDTClassificationLayer::selectedFeatures() const
{
    return featuresList;
}

QgsFeatureRendererV2 *CDTClassificationLayer::renderer()
{
    QVariantMap clsInfo = this->clsInfo();

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

void CDTClassificationLayer::initLayer(const QString &name,
        const QString &methodName,
        const QMap<QString, QVariant> &params,
        const QList<QVariant> &data,
        const QMap<QString, QVariant> &clsInfo,
        const QString &normalizeMethod,
        const QString &pcaParams,
        const QStringList &selectedFeatures)
{
    featuresList = selectedFeatures;
    standardKeyItem()->setText(name);

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

    QList<QPair<QLabel*,QWidget*>> widgets;
    //Transparency
    QSlider *sliderTransparency = new QSlider(Qt::Horizontal,NULL);
    sliderTransparency->setMinimum(0);
    sliderTransparency->setMaximum(100);
    sliderTransparency->setToolTip(tr("Transparency"));
    connect(sliderTransparency,SIGNAL(valueChanged(int)),parent(),SLOT(setLayerTransparency(int)));
    connect(parent(),SIGNAL(layerTransparencyChanged(int)),sliderTransparency,SLOT(setValue(int)));
    connect(this,SIGNAL(destroyed()),sliderTransparency,SLOT(deleteLater()));
    widgets.append(qMakePair(new QLabel(tr("Transparency")),(QWidget*)sliderTransparency));

    this->setWidgetActions(widgets);
}

QList<CDTClassificationLayer *> CDTClassificationLayer::getLayers()
{
    return layers;
}

CDTClassificationLayer *CDTClassificationLayer::getLayer(QUuid id)
{
    foreach (CDTClassificationLayer *layer, layers) {
        if (id == layer->id())
            return layer;
    }
    return NULL;
}

QDataStream &operator<<(QDataStream &out, const CDTClassificationLayer &classification)
{
    out<<classification.id()
      <<classification.name()
     <<classification.method()
    <<classification.params()
    <<classification.data()
    <<classification.clsInfo()
    <<classification.normalizeMethod()
    <<classification.pcaParams()
    <<classification.selectedFeatures();
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTClassificationLayer &classification)
{
    QUuid id;
    QString name;
    QString method;
    QMap<QString, QVariant> param;
    QList<QVariant> data;
    QMap<QString, QVariant> clsInfo;
    QString normalize;
    QString pca;
    QStringList selectedFeatures;

    in>>id>>name>>method>>param>>data>>clsInfo>>normalize>>pca>>selectedFeatures;
    classification.setID(id);
    classification.initLayer(name,method,param,data,clsInfo,normalize,pca,selectedFeatures);

    return in;
}



