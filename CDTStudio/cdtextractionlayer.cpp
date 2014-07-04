#include "cdtextractionlayer.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "qtcolorpicker.h"

QList<CDTExtractionLayer *> CDTExtractionLayer::layers;

CDTExtractionLayer::CDTExtractionLayer(QUuid uuid, QObject *parent) :
    CDTBaseObject(uuid,parent),
    actionChangeColor(new QWidgetAction(this)),
    actionRemoveExtraction(new QAction(QIcon(":/Icon/remove.png"),tr("Remove Extraction"),this)),
    actionRename(new QAction(QIcon(":/Icon/rename.png"),tr("Rename"),this)),
    actionExportShapefile(new QAction(QIcon(":/Icon/export.png"),tr("Export Shapefile"),this)),
    actionStartEdit(new QAction(tr("Start Edit"),this)),
    actionRollBack(new QAction(tr("Roll Back"),this)),
    actionSave(new QAction(tr("Save"),this)),
    actionStop(new QAction(tr("Stop"),this)),
    currentEditState(LOCKED),
    isGeometryModified(false)
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
    connect(actionStartEdit,SIGNAL(triggered()),SLOT(onActionStartEdit()));
    connect(actionRollBack,SIGNAL(triggered()),SLOT(onActionRollBack()));
    connect(actionSave,SIGNAL(triggered()),SLOT(onActionSave()));
    connect(actionStop,SIGNAL(triggered()),SLOT(onActionStop()));

    setEditState(LOCKED);
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

CDTExtractionLayer::EDITSTATE CDTExtractionLayer::editState() const
{
    return currentEditState;
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
    QtColorPicker *borderColorPicker = new QtColorPicker();
    borderColorPicker->setStandardColors();
    borderColorPicker->setCurrentColor(color());
    connect(borderColorPicker,SIGNAL(colorChanged(QColor)),SLOT(setBorderColor(QColor)));
    actionChangeColor->setDefaultWidget(borderColorPicker);

    QMenu *menu =new QMenu(parent);
    menu->addAction(actionChangeColor);
    menu->addAction(actionRename);
    menu->addAction(actionExportShapefile);
    menu->addSeparator();
    menu->addAction(actionRemoveExtraction);
    menu->addSeparator();
    menu->addAction(actionStartEdit);
    menu->addAction(actionRollBack);
    menu->addAction(actionSave);
    menu->addAction(actionStop);
    menu->exec(QCursor::pos());

    actionChangeColor->releaseWidget(borderColorPicker);
    delete borderColorPicker;
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
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE extractionlayer set color = ? where id =?");
    query.bindValue(0,clr);
    query.bindValue(1,this->id().toString());
    query.exec();

    //    setOriginRenderer();
    //    this->mapCanvas->refresh();
    emit extractionChanged();
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
    query.addBindValue(uuid.toString());
    query.addBindValue(name);
    query.addBindValue(shpPath);
    query.addBindValue(color);
    query.addBindValue(((CDTImageLayer*)parent())->id().toString());
    ret = query.exec();
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
    QString id = shapefilePath();
    fileSystem()->exportFiles(id);
}

void CDTExtractionLayer::setEditState(CDTExtractionLayer::EDITSTATE state)
{
    currentEditState = state;

    bool isLocked = currentEditState!=EDITING;
    actionStartEdit->setEnabled(isLocked);
    actionRollBack->setEnabled(!isLocked);
    actionSave->setEnabled(!isLocked);
    actionStop->setEnabled(!isLocked);
}

void CDTExtractionLayer::setGeometryModified(bool modified)
{
    isGeometryModified = modified;

    actionRollBack->setEnabled(modified);
    actionSave->setEnabled(modified);
}

void CDTExtractionLayer::onActionStartEdit()
{
    start();
}

void CDTExtractionLayer::onActionRollBack()
{
    if (isGeometryModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(NULL, tr("Extraction"),
                                   tr("The extraction layer has been modified.\n"
                                      "Do you want to roolback your changes without save?"),
                                   QMessageBox::Reset | QMessageBox::Cancel);
        if (ret == QMessageBox::Reset)
        {
            ///RollBack changes......
            ///
            rollback();
        }
    }
}

void CDTExtractionLayer::onActionSave()
{
    save();
}

void CDTExtractionLayer::onActionStop()
{
    if (isGeometryModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(NULL, tr("Extraction"),
                                   tr("The extraction layer has been modified.\n"
                                      "Do you want to save your changes or ignore changes before stopping editing?"),
                                   QMessageBox::Save | QMessageBox::Ignore| QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            save();
            stop();
        }
        else if (ret == QMessageBox::Ignore)
        {
            stop();
        }
        else
            return;
    }
    else stop();
}

void CDTExtractionLayer::start()
{
    if (currentEditState == EDITING)
        return;
    setEditState(EDITING);
    setGeometryModified(false);
    ///start
    ///
    qDebug()<<"start";
}

void CDTExtractionLayer::rollback()
{
    if (currentEditState == LOCKED)
        return;
    if (isGeometryModified == false)
        return;
    ///rollback
    ///
    setGeometryModified(false);
    qDebug()<<"rollback";
}

void CDTExtractionLayer::save()
{
    if (currentEditState == LOCKED)
        return;
    ///save
    ///
    setGeometryModified(false);
    qDebug()<<"save";
}

void CDTExtractionLayer::stop()
{
    if (currentEditState == LOCKED)
        return;
    /// stop
    ///
    setEditState(LOCKED);
    qDebug()<<"stop";
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
