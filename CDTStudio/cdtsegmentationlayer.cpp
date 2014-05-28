//#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include "cdtimagelayer.h"
#include "cdtproject.h"
#include "cdtattributesdockwidget.h"
#include "cdtmaptoolselecttrainingsamples.h"
#include "stable.h"
#include "dialogdbconnection.h"
#include <qgsvectorlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgssinglesymbolrendererv2.h>
#include <qgsrendererv2widget.h>
#include <qgsfillsymbollayerv2.h>
#include "wizardnewclassification.h"
#include <mainwindow.h>


QDataStream &operator<<(QDataStream &out, const SampleElement &sample)
{
    out<<sample.ObjectID<<sample.categoryID<<sample.sampleID;
    return out;
}

QDataStream &operator>>(QDataStream &in, SampleElement &sample)
{
    in>>sample.ObjectID>>sample.categoryID>>sample.sampleID;
    return in;
}


QList<CDTSegmentationLayer *> CDTSegmentationLayer::layers;

CDTSegmentationLayer::CDTSegmentationLayer(QUuid uuid, QString imagePath,QObject *parent)
    : CDTBaseObject(uuid,parent),
      m_imagePath(imagePath),
      addClassifications(new QAction(tr("Add Classification"),this)),
      actionRemoveSegmentation(new QAction(tr("Remove Segmentation"),this)),
      actionRemoveAllClassifications(new QAction(tr("Remove All Classifications"),this)),
      actionRename(new QAction(tr("Rename Segmentation Name"),this))
{
    layers.push_back(this);

    keyItem   = new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION,CDTProjectTreeItem::VECTOR,QString(),this);
    valueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);

    shapefileItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(
                QList<QStandardItem*>()
                <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,tr("Shapefile path"),this)
                <<shapefileItem);

    markfileItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(
                QList<QStandardItem*>()
                <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,tr("Markfile path"),this)
                <<markfileItem);
    paramRootItem = new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,CDTProjectTreeItem::EMPTY,tr("Method"),this);
    paramRootValueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    keyItem->appendRow(QList<QStandardItem*>()<<paramRootItem<<paramRootValueItem);

    classificationRootItem = new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION_ROOT,CDTProjectTreeItem::GROUP,tr("Classification"),this);
    keyItem->appendRow(classificationRootItem);

    connect(this,SIGNAL(nameChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(methodParamsChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(removeSegmentation(CDTSegmentationLayer*)),this->parent(),SLOT(removeSegmentation(CDTSegmentationLayer*)));
    connect(this,SIGNAL(segmentationChanged()),this->parent(),SIGNAL(imageLayerChanged()));

    connect(addClassifications,SIGNAL(triggered()),this,SLOT(addClassification()));
    connect(actionRemoveSegmentation,SIGNAL(triggered()),this,SLOT(remove()));
    connect(actionRemoveAllClassifications,SIGNAL(triggered()),this,SLOT(removeAllClassifications()));
    connect(actionRename,SIGNAL(triggered()),this,SLOT(onActionRename()));
}

CDTSegmentationLayer::~CDTSegmentationLayer()
{
    layers.removeAll(this);
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from segmentationlayer where id = '"+uuid.toString()+"'");
    if (!ret)
        qDebug()<<"prepare:"<<query.lastError().text();
}

void CDTSegmentationLayer::onContextMenuRequest(QWidget *parent)
{
    actionRemoveSegmentation->setIcon(QIcon(":/Icon/remove.png"));
    actionRemoveAllClassifications->setIcon(QIcon(":/Icon/remove.png"));
    addClassifications->setIcon(QIcon(":/Icon/add.png"));
    actionRename->setIcon(QIcon(":/Icon/rename.png"));
    QMenu *menu =new QMenu(parent);
    menu->addAction(addClassifications);
    menu->addAction(actionRemoveSegmentation);
    menu->addAction(actionRemoveAllClassifications);
    menu->addSeparator();
    menu->addAction(actionRename);
    menu->exec(QCursor::pos());
}

void CDTSegmentationLayer::onActionRename()
{
    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input Segmentation Name"),
                tr("Segmentation name:"), QLineEdit::Normal,
                this->name(), &ok);
    if (ok && !text.isEmpty())
        setName(text);
}

void CDTSegmentationLayer::addClassification()
{
    MainWindow::getAttributesDockWidget()->clearTables();
    WizardNewClassification dlg(id());
    int ret = dlg.exec();
    MainWindow::getAttributesDockWidget()->updateTable();

    if (ret == QWizard::Accepted || dlg.isValid())
    {
        CDTClassification *classification = new CDTClassification(QUuid::createUuid(),this);
        classification->initClassificationLayer(
            dlg.name,
            dlg.method,
            dlg.params,
            dlg.label,
            dlg.categoryID_Index);
        classificationRootItem->appendRow(classification->standardItems());
        addClassification(classification);
    }
}

void CDTSegmentationLayer::remove()
{
    emit removeSegmentation(this);
}

//void CDTSegmentationLayer::removeClassification(CDTClassification* clf)
//{
//    for(int i =0;i <classifications.size();++i)
//    {
//        if(clf->name() == classifications[i]->name())
//        {
//            classifications.remove(i);
//            emit segmentationChanged();
//            connect(clf,SIGNAL(methodParamsChanged()),this,SIGNAL(segmentationChanged()));

//        }
//    }
//}

//void CDTSegmentationLayer::removeAllClassifications()
//{
//    classifications.clear();
//    emit segmentationChanged();
//}


QString CDTSegmentationLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from segmentationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTSegmentationLayer::shapefilePath() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select shapefilePath from segmentationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTSegmentationLayer::markfilePath() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select markfilePath from segmentationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTSegmentationLayer::method() const
{
    return m_method;
}

CDTDatabaseConnInfo CDTSegmentationLayer::databaseURL() const
{
    return m_dbUrl;
}

QString CDTSegmentationLayer::imagePath() const
{
    return m_imagePath;
}

void CDTSegmentationLayer::setClassificationInfo(CDTClassification *classification)
{
    classification->data();
    classification->clsInfo();

    QgsVectorLayer*p = (QgsVectorLayer*)mapCanvasLayer;
    QgsFields fields = p->pendingFields();
    qDebug()<<fields.indexFromName("ClassID");
    if (fields.indexFromName("ClassID")==-1)
    {
        QgsField field("ClassID",QVariant::String);
        fields.append(field);
    }

}

void CDTSegmentationLayer::setRenderer(QgsFeatureRendererV2* r)
{
    QgsVectorLayer*p = (QgsVectorLayer*)mapCanvasLayer;
    if (p!=NULL)
    {
        p->setRendererV2(r);
    }
}

QList<CDTSegmentationLayer *> CDTSegmentationLayer::getLayers()
{
    return layers;
}

CDTSegmentationLayer *CDTSegmentationLayer::getLayer(QUuid id)
{
    foreach (CDTSegmentationLayer *layer, layers) {
        if (id == layer->uuid)
            return layer;
    }
    return NULL;
}

void CDTSegmentationLayer::setName(const QString &name)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE segmentationlayer set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit nameChanged();
}

void CDTSegmentationLayer::setLayerInfo(const QString &name, const QString &shpPath, const QString &mkPath)
{
    QgsVectorLayer *newLayer = new QgsVectorLayer(/*QFileInfo(shpPath).absolutePath()*/shpPath,QFileInfo(shpPath).completeBaseName(),"ogr");
    if (!newLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open shapefile ")+shpPath+tr(" failed!"));
        delete newLayer;
        return;
    }

    if (mapCanvasLayer)
        delete mapCanvasLayer;
    mapCanvasLayer = newLayer;

    keyItem->setText(name);
    shapefileItem->setText(shpPath);
    markfileItem->setText(mkPath);


    QgsSimpleFillSymbolLayerV2* symbolLayer = new QgsSimpleFillSymbolLayerV2();
    symbolLayer->setColor(QColor(0,0,0,0));
    symbolLayer->setBorderColor(QColor(qrand()%255,qrand()%255,qrand()%255));
    QgsFillSymbolV2 *fillSymbol = new QgsFillSymbolV2(QgsSymbolLayerV2List()<<symbolLayer);
    QgsSingleSymbolRendererV2* singleSymbolRenderer = new QgsSingleSymbolRendererV2(fillSymbol);
    this->setRenderer(singleSymbolRenderer);


    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer);
    keyItem->setMapLayer(mapCanvasLayer);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into segmentationlayer VALUES(?,?,?,?,?)"); 
    query.bindValue(0,uuid.toString());
    query.bindValue(1,name);
    query.bindValue(2,shpPath);
    query.bindValue(3,mkPath);
    query.bindValue(4,((CDTImageLayer*)parent())->id().toString());
    query.exec();

    emit appendLayers(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit segmentationChanged();
}

void CDTSegmentationLayer::setMethodParams(const QString &methodName, const QMap<QString, QVariant> &params)
{
    m_method = methodName;
    paramRootItem->removeRows(0,paramRootItem->rowCount());
    paramRootValueItem->setText(m_method);
    m_params = params;
    QStringList keys = m_params.keys();
    foreach (QString key, keys) {
        QVariant value = m_params.value(key);
        paramRootItem->appendRow(
                    QList<QStandardItem*>()
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,key,this)
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,value.toString(),this)
                    );
    }
}

void CDTSegmentationLayer::setDatabaseURL(CDTDatabaseConnInfo url)
{
    m_dbUrl = url;
    emit segmentationChanged();
}

void CDTSegmentationLayer::addClassification(CDTClassification *classification)
{
    classifications.push_back(classification);
    emit segmentationChanged();
}

void CDTSegmentationLayer::loadSamplesFromStruct(const QMap<QString, QString> &sample_id_name, const QList<SampleElement> &samples)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("insert into sample_segmentation values(?,?,?)");
    foreach (QString id, sample_id_name.keys()) {
        QString name = sample_id_name.value(id);
        query.bindValue(0,id);
        query.bindValue(1,name);
        query.bindValue(2,this->id().toString());
        query.exec();
    }

    query.prepare("insert into samples values(?,?,?)");
    foreach (SampleElement sample, samples) {
        query.bindValue(0,sample.ObjectID);
        query.bindValue(1,sample.categoryID.toString());
        query.bindValue(2,sample.sampleID.toString());
        query.exec();
    }
}

void CDTSegmentationLayer::saveSamplesToStruct(QMap<QString, QString> &sample_id_name, QList<SampleElement> &samples) const
{
    sample_id_name.clear();
    samples.clear();

    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select id,name from sample_segmentation where segmentationid ='" + this->id().toString() +"'");
    while(query.next())
    {       
        sample_id_name.insert(query.value(0).toString(),query.value(1).toString());
    }

    foreach (QString sampleID, sample_id_name.keys()) {
        query.exec("select objectid,categoryid from samples where sampleid ='" + sampleID+"'");
        while(query.next())
        {
            samples<<SampleElement(query.value(0).toInt(),query.value(1).toString(),sampleID);
        }
    }
}

QDataStream &operator<<(QDataStream &out, const CDTSegmentationLayer &segmentation)
{    
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from segmentationlayer where id ='" + segmentation.id().toString() +"'");
    query.next();

    out <<segmentation.uuid         //id
        <<query.value(1).toString() //name
        <<query.value(2).toString() //shapfile
        <<query.value(3).toString() //markfile
        <<segmentation.m_method<<segmentation.m_params<<segmentation.m_dbUrl;

    QMap<QString,QString> sample;
    QList<SampleElement> samples;
    segmentation.saveSamplesToStruct(sample,samples);
    out<<sample<<samples;
    out<<segmentation.classifications.size();
    for (int i=0;i<segmentation.classifications.size();++i)
        out<<*(segmentation.classifications[i]);

    return out;
}


QDataStream &operator>>(QDataStream &in,CDTSegmentationLayer &segmentation)
{
    in>>segmentation.uuid;

    QString name,shp,mark;
    in>>name>>shp>>mark;
    segmentation.setLayerInfo(name,shp,mark);

    QString temp;
    in>>temp;

    QMap<QString,QVariant> paramsTemp;
    in>>paramsTemp;
    segmentation.setMethodParams(temp,paramsTemp);
    in>>segmentation.m_dbUrl;


    QMap<QString,QString> sample;
    QList<SampleElement> samples;
    in>>sample>>samples;
    segmentation.loadSamplesFromStruct(sample,samples);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTClassification* classification = new CDTClassification(QUuid(),&segmentation);
        in>>*classification;
        segmentation.classificationRootItem->appendRow(classification->standardItems());
        segmentation.classifications.push_back(classification);
    }
    return in;
}
