#include "cdtextractionlayer.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "qtcolorpicker.h"

QList<CDTExtractionLayer *> CDTExtractionLayer::layers;

CDTExtractionLayer::CDTExtractionLayer(QUuid uuid, QObject *parent) :
    CDTBaseObject(uuid,parent),
    actionChangeParams(new QWidgetAction(this)),
    actionRemoveExtraction(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove Extraction"),this)),
    actionRename(new QAction(QIcon(":/Icon/Rename.png"),tr("Rename"),this)),
    actionExportShapefile(new QAction(QIcon(":/Icon/Export.png"),tr("Export Shapefile"),this))
{
    layers.push_back(this);

    keyItem   = new CDTProjectTreeItem(CDTProjectTreeItem::EXTRACTION,CDTProjectTreeItem::VECTOR,QString(),this);
    valueItem = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);

    connect(this,SIGNAL(removeExtraction(CDTExtractionLayer*)),this->parent(),SLOT(removeExtraction(CDTExtractionLayer*)));
    connect(this,SIGNAL(nameChanged()),this,SIGNAL(extractionChanged()));
    connect(this,SIGNAL(extractionChanged()),this->parent(),SIGNAL(imageLayerChanged()));

    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
    connect(actionRemoveExtraction,SIGNAL(triggered()),SLOT(remove()));
    connect(actionExportShapefile,SIGNAL(triggered()),SLOT(exportShapefile()));
}

CDTExtractionLayer::~CDTExtractionLayer()
{
    if (id().isNull())
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from extractionlayer where id = '"+uuid.toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
    layers.removeAll(this);
}

QString CDTExtractionLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTExtractionLayer::shapefileID() const
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

QColor CDTExtractionLayer::borderColor() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select borderColor from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
//    qDebug()<<query.value(0);
    return query.value(0).value<QColor>();
}

double CDTExtractionLayer::opacity() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select opacity from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
    qDebug()<<query.value(0);
    return query.value(0).toDouble();
}

//QString CDTExtractionLayer::imagePath() const
//{
//    return ((CDTImageLayer*)parent())->path();
//}

void CDTExtractionLayer::setRenderer(QgsFeatureRendererV2 *r)
{
    QgsVectorLayer*p = (QgsVectorLayer*)mapCanvasLayer;
    if (p!=NULL)
    {
        p->setRendererV2(r);
    }
}

void CDTExtractionLayer::setOriginRenderer()
{
    QgsSimpleFillSymbolLayerV2* symbolLayer = new QgsSimpleFillSymbolLayerV2();
    symbolLayer->setColor(color());
    symbolLayer->setBorderColor(borderColor());
    QgsFillSymbolV2 *fillSymbol = new QgsFillSymbolV2(QgsSymbolLayerV2List()<<symbolLayer);
    QgsSingleSymbolRendererV2* singleSymbolRenderer = new QgsSingleSymbolRendererV2(fillSymbol);
    this->setRenderer(singleSymbolRenderer);
}

QList<CDTExtractionLayer *> CDTExtractionLayer::getLayers()
{
    return layers;
}

CDTExtractionLayer *CDTExtractionLayer::getLayer(QUuid id)
{
    foreach (CDTExtractionLayer *layer, layers) {
        if (id == layer->uuid)
            return layer;
    }
    return NULL;
}

void CDTExtractionLayer::onContextMenuRequest(QWidget *parent)
{
    QWidget* menuWidget = new QWidget(NULL);
    QFormLayout* layout = new QFormLayout(menuWidget);
    menuWidget->setLayout(layout);

    QtColorPicker *colorPicker = new QtColorPicker(menuWidget);
    colorPicker->setStandardColors();
    colorPicker->setCurrentColor(color());
    connect(colorPicker,SIGNAL(colorChanged(QColor)),SLOT(setColor(QColor)));

    QtColorPicker *borderColorPicker = new QtColorPicker(menuWidget);
    borderColorPicker->setStandardColors();
    borderColorPicker->setCurrentColor(borderColor());
    connect(borderColorPicker,SIGNAL(colorChanged(QColor)),SLOT(setBorderColor(QColor)));

    layout->addRow(tr("Color"),colorPicker);
    layout->addRow(tr("Border Color"),borderColorPicker);
    actionChangeParams->setDefaultWidget(menuWidget);

    QMenu *menu =new QMenu(parent);
    menu->addAction(actionChangeParams);
    menu->addAction(actionRename);
    menu->addAction(actionExportShapefile);
    menu->addSeparator();
    menu->addAction(actionRemoveExtraction);
    //    menu->addSeparator();
    //    menu->addAction(actionStartEdit);
    //    menu->addAction(actionRollBack);
    //    menu->addAction(actionSave);
    //    menu->addAction(actionStop);
    menu->exec(QCursor::pos());

    actionChangeParams->releaseWidget(menuWidget);
    delete menuWidget;
}

void CDTExtractionLayer::setName(const QString &name)
{
    if (this->name()==name)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE extractionlayer set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit nameChanged();
}

void CDTExtractionLayer::setColor(const QColor &clr)
{
    if (this->color() == clr)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE extractionlayer set color = ? where id =?");
    query.bindValue(0,clr);
    query.bindValue(1,this->id().toString());
    query.exec();

    setOriginRenderer();
    this->mapCanvas->refresh();
    emit extractionChanged();
}

void CDTExtractionLayer::setBorderColor(const QColor &clr)
{
    if (this->borderColor() == clr)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE extractionlayer set bordercolor = ? where id =?");
    query.bindValue(0,clr);
    query.bindValue(1,this->id().toString());
    query.exec();

    setOriginRenderer();
    this->mapCanvas->refresh();
    emit extractionChanged();
}

void CDTExtractionLayer::setOpacity(const double &val)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE extractionlayer set Opacity = ? where id =?");
    query.bindValue(0,val);
    query.bindValue(1,this->id().toString());
    query.exec();

    //    setOriginRenderer();
    //    this->mapCanvas->refresh();
    emit extractionChanged();
}

void CDTExtractionLayer::setOpacity(const int &val)
{
    setOpacity(val/100.);
}

void CDTExtractionLayer::initLayer(const QString &name, const QString &shpID,
                                   const QColor &color, const QColor &borderColor, double opacity)
{
    QString tempShpPath;
    this->fileSystem()->getFile(shpID,tempShpPath);
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
    ret = query.prepare("insert into extractionlayer VALUES(?,?,?,?,?,?,?)");
    query.addBindValue(uuid.toString());
    query.addBindValue(name);
    query.addBindValue(shpID);
    query.addBindValue(color);
    query.addBindValue(borderColor);
    query.addBindValue(opacity);
    query.addBindValue(((CDTImageLayer*)parent())->id().toString());
    ret = query.exec();

    setOriginRenderer();

    emit appendLayers(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit extractionChanged();
}

void CDTExtractionLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input Extraction Name"),
                tr("Extraction rename:"), QLineEdit::Normal,
                this->name(), &ok);
    if (ok && !text.isEmpty())
        setName(text);
}

void CDTExtractionLayer::remove()
{
    emit removeExtraction(this);
}

void CDTExtractionLayer::exportShapefile()
{
    QString id = shapefileID();
    fileSystem()->exportFiles(id);
}

QDataStream &operator<<(QDataStream &out, const CDTExtractionLayer &extraction)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from extractionlayer where id ='" + extraction.id().toString() +"'");
    query.next();

    out<<extraction.id()//id
      <<query.value(1).toString()       //name
     <<query.value(2).toString()        //shapefile
    <<query.value(3).value<QColor>()    //color
    <<query.value(4).value<QColor>()    //border color
    <<query.value(5).toDouble();        //opacity

    return out;
}


QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction)
{
    in>>extraction.uuid;
    QString name,shp;
    in>>name>>shp;
    QColor color,borderColor;
    in>>color>>borderColor;
    double opacity;
    in>>opacity;

    extraction.initLayer(name,shp,color,borderColor,opacity);
    return in;
}
