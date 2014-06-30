#include "cdtsegmentationlayer.h"
#include "stable.h"
#include "cdtproject.h"
#include "cdtimagelayer.h"
#include "cdtclassification.h"
#include "cdtattributesdockwidget.h"
#include "cdtmaptoolselecttrainingsamples.h"
#include "wizardnewclassification.h"
#include "cdtvariantconverter.h"
#include "mainwindow.h"
#include "qtcolorpicker.h"
#include "cdtfilesystem.h"
#include "cdtprojecttreeitem.h"

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

CDTSegmentationLayer::CDTSegmentationLayer(QUuid uuid, QObject *parent)
    : CDTBaseObject(uuid,parent),
      actionAddClassifications(new QAction(QIcon(":/Icon/add.png"),tr("Add Classification"),this)),
      actionRemoveSegmentation(new QAction(QIcon(":/Icon/remove.png"),tr("Remove Segmentation"),this)),
      actionRemoveAllClassifications(new QAction(QIcon(":/Icon/remove.png"),tr("Remove All Classifications"),this)),
      actionRename(new QAction(QIcon(":/Icon/rename.png"),tr("Rename Segmentation"),this)),
      actionChangeBorderColor(new QWidgetAction(this))
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

    classificationRootItem = new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION_ROOT,CDTProjectTreeItem::EMPTY,tr("Classification"),this);
    keyItem->appendRow(classificationRootItem);    

    connect(this,SIGNAL(nameChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(methodParamsChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(removeSegmentation(CDTSegmentationLayer*)),this->parent(),SLOT(removeSegmentation(CDTSegmentationLayer*)));
    connect(this,SIGNAL(segmentationChanged()),this->parent(),SIGNAL(imageLayerChanged()));

    connect(actionRemoveSegmentation,SIGNAL(triggered()),this,SLOT(remove()));
    connect(actionAddClassifications,SIGNAL(triggered()),this,SLOT(addClassification()));
    connect(actionRemoveAllClassifications,SIGNAL(triggered()),this,SLOT(removeAllClassifications()));
    connect(actionRename,SIGNAL(triggered()),this,SLOT(rename()));
}

CDTSegmentationLayer::~CDTSegmentationLayer()
{    
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from segmentationlayer where id = '"+uuid.toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    layers.removeAll(this);
}

void CDTSegmentationLayer::onContextMenuRequest(QWidget *parent)
{    
    QtColorPicker *borderColorPicker = new QtColorPicker();
    borderColorPicker->setStandardColors();
    borderColorPicker->setCurrentColor(color());
    connect(borderColorPicker,SIGNAL(colorChanged(QColor)),SLOT(setBorderColor(QColor)));
    actionChangeBorderColor->setDefaultWidget(borderColorPicker);

    QMenu *menu =new QMenu(parent);
    menu->addAction(actionChangeBorderColor);
    menu->addAction(actionRename);
    menu->addSeparator();
    menu->addAction(actionRemoveSegmentation);
    menu->addSeparator();
    menu->addAction(actionAddClassifications);
    menu->addAction(actionRemoveAllClassifications);    
    menu->addSeparator();    
    menu->exec(QCursor::pos());

    actionChangeBorderColor->releaseWidget(borderColorPicker);
    delete borderColorPicker;
}

void CDTSegmentationLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input Segmentation Name"),
                tr("Segmentation rename:"), QLineEdit::Normal,
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
                    dlg.categoryID_Index,
                    dlg.normalizeMethod,
                    dlg.pcaParams);
        classificationRootItem->appendRow(classification->standardItems());
        addClassification(classification);
    }
}

void CDTSegmentationLayer::remove()
{
    emit removeSegmentation(this);
}

void CDTSegmentationLayer::removeClassification(CDTClassification* clf)
{
    int index = classifications.indexOf(clf);
    if (index>=0)
    {
        QStandardItem* keyItem = clf->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        classifications.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<clf->canvasLayer());
        delete clf;
        emit segmentationChanged();
    }
}

void CDTSegmentationLayer::removeAllClassifications()
{
    foreach (CDTClassification* clf, classifications) {
        QStandardItem* keyItem = clf->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        emit removeLayer(QList<QgsMapLayer*>()<<clf->canvasLayer());
        delete clf;
    }
    classifications.clear();
    emit segmentationChanged();
}


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

QString CDTSegmentationLayer::shapefileTempPath() const
{
    QString temp;
    fileSystem()->getFile(shapefilePath(),temp);
    return temp;
}

QString CDTSegmentationLayer::markfileTempPath() const
{
    QString temp;
    fileSystem()->getFile(markfilePath(),temp);
    return temp;
}

QString CDTSegmentationLayer::method() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select method from segmentationlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

CDTDatabaseConnInfo CDTSegmentationLayer::databaseURL() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select dbUrl from segmentationlayer where id ='" + this->id().toString() +"'");
    query.next();
    CDTDatabaseConnInfo url = variantToData<CDTDatabaseConnInfo>(query.value(0));
    return url;
}

QColor CDTSegmentationLayer::color() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select color from segmentationlayer where id ='" + this->id().toString() +"'");
    query.next();
    qDebug()<<query.value(0);
    return query.value(0).value<QColor>();
}

QString CDTSegmentationLayer::imagePath() const
{
    return ((CDTImageLayer*)parent())->path();
}

void CDTSegmentationLayer::setRenderer(QgsFeatureRendererV2* r)
{
    QgsVectorLayer*p = (QgsVectorLayer*)mapCanvasLayer;
    if (p!=NULL)
    {
        p->setRendererV2(r);
    }
}

void CDTSegmentationLayer::setOriginRenderer()
{
    QgsSimpleFillSymbolLayerV2* symbolLayer = new QgsSimpleFillSymbolLayerV2();
    symbolLayer->setColor(QColor(0,0,0,0));
    symbolLayer->setBorderColor(color());
    QgsFillSymbolV2 *fillSymbol = new QgsFillSymbolV2(QgsSymbolLayerV2List()<<symbolLayer);
    QgsSingleSymbolRendererV2* singleSymbolRenderer = new QgsSingleSymbolRendererV2(fillSymbol);
    this->setRenderer(singleSymbolRenderer);
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

void CDTSegmentationLayer::setBorderColor(const QColor &clr)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE segmentationlayer set color = ? where id =?");
    query.bindValue(0,clr);
    query.bindValue(1,this->id().toString());
    query.exec();

    setOriginRenderer();
    this->mapCanvas->refresh();
    emit segmentationChanged();
}

void CDTSegmentationLayer::initSegmentationLayer(const QString &name,
                                                 const QString &shpPath,
                                                 const QString &mkPath,
                                                 const QString &method,
                                                 const QVariantMap &params,
                                                 CDTDatabaseConnInfo url,
                                                 const QColor &color)
{
    QString tempShpPath;
    this->fileSystem()->getFile(shpPath,tempShpPath);
    QgsVectorLayer *newLayer = new QgsVectorLayer(/*shpPath*/tempShpPath,QFileInfo(/*shpPath*/tempShpPath).completeBaseName(),"ogr");
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
    shapefileItem->setText(shpPath);
    markfileItem->setText(mkPath);

    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer);
    keyItem->setMapLayer(mapCanvasLayer);

    //set method $ params
    paramRootItem->removeRows(0,paramRootItem->rowCount());
    paramRootValueItem->setText(method);
    QStringList keys = params.keys();
    foreach (QString key, keys) {
        QVariant value = params.value(key);
        paramRootItem->appendRow(
                    QList<QStandardItem*>()
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,key,this)
                    <<new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,value.toString(),this)
                    );
    }
    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into segmentationlayer VALUES(?,?,?,?,?,?,?,?,?)");
    query.bindValue(0,uuid.toString());
    query.bindValue(1,name);
    query.bindValue(2,shpPath);
    query.bindValue(3,mkPath);
    query.bindValue(4,method);
    query.bindValue(5,dataToVariant(params));

    query.bindValue(6,dataToVariant(url));
    query.bindValue(7,color);
    query.bindValue(8,((CDTImageLayer*)parent())->id().toString());
    query.exec();

    setOriginRenderer();

    emit appendLayers(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit segmentationChanged();
}

void CDTSegmentationLayer::setDatabaseURL(CDTDatabaseConnInfo url)
{
    //    m_dbUrl = url;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE segmentationlayer set dbUrl = ? where id =?");
    query.bindValue(0,dataToVariant(url));
    query.bindValue(1,this->id().toString());
    query.exec();
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

    out <<segmentation.id()         //id
       <<query.value(1).toString() //name
      <<query.value(2).toString() //shapfile
     <<query.value(3).toString() //markfile
    <<query.value(4).toString() //method
    <<query.value(5)//params
    <<query.value(6)//dbUrl
    <<query.value(7);//Border Color

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

    QString name,shp,mark,method;
    in>>name>>shp>>mark>>method;
    QVariant temp;
    in>>temp;
    QVariantMap params = variantToData<QVariantMap>(temp);
    in>>temp;
    CDTDatabaseConnInfo url = variantToData<CDTDatabaseConnInfo>(temp);
    in>>temp;
    QColor color = temp.value<QColor>();

    segmentation.initSegmentationLayer(name,shp,mark,method,params,url,color);


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
