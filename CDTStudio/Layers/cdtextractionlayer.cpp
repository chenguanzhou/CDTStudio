#include "cdtextractionlayer.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "qtcolorpicker.h"

QList<CDTExtractionLayer *> CDTExtractionLayer::layers;

CDTExtractionLayer::CDTExtractionLayer(QUuid uuid, QObject *parent) :
    CDTBaseLayer(uuid,parent)
{
    layers.push_back(this);

    CDTProjectTreeItem *keyItem =
            new CDTProjectTreeItem(CDTProjectTreeItem::EXTRACTION,CDTProjectTreeItem::VECTOR,QString(),this);
    setKeyItem(keyItem);

    QAction *actionRename =
            new QAction(QIcon(":/Icons/Rename.png"),tr("Rename"),this);
    QAction *actionExportShapefile =
            new QAction(QIcon(":/Icons/Export.png"),tr("Export Shapefile"),this);
    QAction *actionRemoveExtraction =
            new QAction(QIcon(":/Icons/Remove.png"),tr("Remove Extraction"),this);

    QList<QList<QAction*> > actions = QList<QList<QAction*> >()
            <<(QList<QAction*>()<<actionRename<<actionExportShapefile)
           <<(QList<QAction*>()<<actionRemoveExtraction);
    this->setActions(actions);

    connect(this,SIGNAL(removeExtraction(CDTExtractionLayer*)),this->parent(),SLOT(removeExtraction(CDTExtractionLayer*)));
    connect(this,SIGNAL(nameChanged()),this,SIGNAL(layerChanged()));

    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));    
    connect(actionExportShapefile,SIGNAL(triggered()),SLOT(exportShapefile()));
    connect(actionRemoveExtraction,SIGNAL(triggered()),SLOT(remove()));
}

CDTExtractionLayer::~CDTExtractionLayer()
{
    if (id().isNull())
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from extractionlayer where id = '"+id().toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
    layers.removeAll(this);
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
    return query.value(0).value<QColor>();
}

QColor CDTExtractionLayer::borderColor() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select borderColor from extractionlayer where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).value<QColor>();
}

int CDTExtractionLayer::layerTransparency() const
{
    QgsVectorLayer*p = qobject_cast<QgsVectorLayer*>(canvasLayer());
    if (p)
        return p->layerTransparency();
    else
        return -1;
}

void CDTExtractionLayer::setRenderer(QgsFeatureRendererV2 *r)
{
    QgsVectorLayer*p = (QgsVectorLayer*)canvasLayer();
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
        if (id == layer->id())
            return layer;
    }
    return NULL;
}

//void CDTExtractionLayer::onContextMenuRequest(QWidget *parent)
//{
//    QWidget* menuWidget = new QWidget(NULL);
//    QFormLayout* layout = new QFormLayout(menuWidget);
//    menuWidget->setLayout(layout);
//    layout->setSpacing(1);

//    QtColorPicker *colorPicker = new QtColorPicker(menuWidget);
//    colorPicker->setStandardColors();
//    colorPicker->setCurrentColor(color());
//    connect(colorPicker,SIGNAL(colorChanged(QColor)),SLOT(setColor(QColor)));
//    connect(this,SIGNAL(colorChanged(QColor)),colorPicker,SLOT(setCurrentColor(QColor)));

//    QtColorPicker *borderColorPicker = new QtColorPicker(menuWidget);
//    borderColorPicker->setStandardColors();
//    borderColorPicker->setCurrentColor(borderColor());
//    connect(borderColorPicker,SIGNAL(colorChanged(QColor)),SLOT(setBorderColor(QColor)));
//    connect(this,SIGNAL(borderColorChanged(QColor)),borderColorPicker,SLOT(setCurrentColor(QColor)));

//    layout->addRow(tr("Color"),colorPicker);
//    layout->addRow(tr("Border Color"),borderColorPicker);
//    actionChangeParams->setDefaultWidget(menuWidget);

//    QMenu *menu =new QMenu(parent);
//    menu->addAction(actionChangeParams);
//    menu->addAction(actionRename);
//    menu->addAction(actionExportShapefile);
//    menu->addSeparator();
//    menu->addAction(actionRemoveExtraction);
//    menu->exec(QCursor::pos());

//    actionChangeParams->releaseWidget(menuWidget);
//    delete menuWidget;
//}

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
    this->canvas()->refresh();

    emit colorChanged(clr);
    emit layerChanged();
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
    this->canvas()->refresh();

    emit borderColorChanged(clr);
    emit layerChanged();
}

void CDTExtractionLayer::setLayerTransparency(const int &transparency)
{
    QgsVectorLayer*p = qobject_cast<QgsVectorLayer*>(canvasLayer());
    if (p)
    {
        p->setLayerTransparency(transparency);
        canvas()->refresh();
    }
}

void CDTExtractionLayer::initLayer(const QString &name, const QString &shpID,
                                   const QColor &color, const QColor &borderColor)
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

    setCanvasLayer(newLayer);
    connect(newLayer,SIGNAL(layerTransparencyChanged(int)),this,SIGNAL(layerTransparencyChanged(int)));

    keyItem()->setText(name);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into extractionlayer VALUES(?,?,?,?,?,?)");
    if (ret==false)
    {
        qDebug()<<"Prepare 'insert into extractionlayer failed!'";
        return;
    }
    query.addBindValue(id().toString());
    query.addBindValue(name);
    query.addBindValue(shpID);
    query.addBindValue(color);
    query.addBindValue(borderColor);
    query.addBindValue(((CDTImageLayer*)parent())->id().toString());
    ret = query.exec();
    if (ret==false)
    {
        qDebug()<<"insert into extractionlayer failed!";
        return;
    }

    QList<QPair<QLabel*,QWidget*>> widgets;
    QtColorPicker *colorPicker = new QtColorPicker();
    colorPicker->setStandardColors();
    colorPicker->setCurrentColor(color);
    connect(colorPicker,SIGNAL(colorChanged(QColor)),SLOT(setColor(QColor)));
    connect(this,SIGNAL(colorChanged(QColor)),colorPicker,SLOT(setCurrentColor(QColor)));
    widgets.append(qMakePair(new QLabel(tr("Color")),(QWidget*)colorPicker));

    QtColorPicker *borderColorPicker = new QtColorPicker();
    borderColorPicker->setStandardColors();
    borderColorPicker->setCurrentColor(borderColor);
    connect(borderColorPicker,SIGNAL(colorChanged(QColor)),SLOT(setBorderColor(QColor)));
    connect(this,SIGNAL(borderColorChanged(QColor)),borderColorPicker,SLOT(setCurrentColor(QColor)));
    widgets.append(qMakePair(new QLabel(tr("Border color")),(QWidget*)borderColorPicker));

    QSlider *sliderTransparency = new QSlider(Qt::Horizontal,NULL);
    sliderTransparency->setMinimum(0);
    sliderTransparency->setMaximum(100);
    sliderTransparency->setToolTip(tr("Transparency"));
    connect(sliderTransparency,SIGNAL(valueChanged(int)),SLOT(setLayerTransparency(int)));
    connect(this,SIGNAL(layerTransparencyChanged(int)),sliderTransparency,SLOT(setValue(int)));
    connect(this,SIGNAL(destroyed()),sliderTransparency,SLOT(deleteLater()));
    widgets.append(qMakePair(new QLabel(tr("Transparency")),(QWidget*)sliderTransparency));
    setWidgetActions(widgets);

    setWidgetActions(widgets);

    setOriginRenderer();

    emit appendLayers(QList<QgsMapLayer*>()<<canvasLayer());
    emit layerChanged();
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
    <<-1;        //deprecated

    return out;
}


QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction)
{
    QUuid id;
    in>>id;
    extraction.setID(id);
    QString name,shp;
    in>>name>>shp;
    QColor color,borderColor;
    in>>color>>borderColor;
    double opacity;
    in>>opacity;

    extraction.initLayer(name,shp,color,borderColor);
    return in;
}
