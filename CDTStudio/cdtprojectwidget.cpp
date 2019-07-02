#include "cdtprojectwidget.h"
#include "cdtprojecttabwidget.h"
#include "cdtbaselayer.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtapplication.h"

CDTProjectWidget::CDTProjectWidget(QWidget *parent) :
    QWidget(parent),
    project(Q_NULLPTR),
    treeModelObject(new QStandardItemModel(this)),
    mapCanvas(new QgsMapCanvas(this))
{
    connect(treeModelObject,SIGNAL(itemChanged(QStandardItem*)),SLOT(onObjectItemChanged(QStandardItem*)));

    connect(this,SIGNAL(projectChanged()),this,SLOT(onProjectChanged()));
    connect(mapCanvas,SIGNAL(xyCoordinates(QgsPointXY)),MainWindow::getMainWindow(),SLOT(showMouseCoordinate(QgsPointXY)));
    connect(mapCanvas,SIGNAL(scaleChanged(double)),MainWindow::getMainWindow(),SLOT(showScale(double)));
    connect(mapCanvas,SIGNAL(renderStarting()),SLOT(onRenderStarting()));
    connect(mapCanvas,SIGNAL(renderComplete(QPainter*)),SLOT(onRenderComplete()));
    QVBoxLayout *vbox = new QVBoxLayout(this);
    mapCanvas->enableAntiAliasing(true);
    mapCanvas->setFrameStyle(QFrame::Plain);
    mapCanvas->setCanvasColor(this->palette().color(this->backgroundRole()));

    vbox->addWidget(mapCanvas);
    vbox->setMargin(0);
    this->setLayout(vbox);

    QToolBar* toolBar = initToolBar();
    if (toolBar != Q_NULLPTR)
        vbox->setMenuBar(toolBar);
}

CDTProjectWidget::~CDTProjectWidget()
{
    MainWindow::getMainWindow()->clearAllDocks();
    if(project)
    {
        project->removeAllImageLayers();
        delete project;
    }
    file.close();
}

//void CDTProjectWidget::onContextMenu(QPoint , QModelIndex index)
//{
//    CDTProjectTreeItem *item =(CDTProjectTreeItem*)treeModelObject->itemFromIndex(index);
//    if(item ==NULL)
//        return;
////    int type = item->getType();
//}

bool CDTProjectWidget::readProject(const QString &filepath)
{
    QFileInfo info(filepath);
    if(!info.exists())
    {
        qWarning()<<filepath<<" is not exist!";
        return false;
    }

    if (openProjectFile(filepath)==false)
        return false;

    QTime time;
    time.start();

//    QTemporaryFile tempFile;
//    tempFile.open();
//    tempFile.write(data);
//    tempFile.flush();
//    tempFile.seek(0);
//    QDataStream in(&(tempFile));

    QDataStream in(qUncompress(file.readAll()));

    quint32 magicNumber;
    in>>  magicNumber;
    if (magicNumber != (quint32)0xABCDEF)
    {
        qWarning()<<"Magic number is not match.It is :"<<magicNumber;
        return false;
    }
    createProject(QUuid());
    project->setPath(filepath);
    in>>*project;
    emit projectChanged();
    setWindowModified(false);

    qDebug("Open the project cost %1 ms",time.elapsed());

    refreshMapCanvas();
    return true;
}

bool CDTProjectWidget::writeProject()
{
    QTime time;
    time.start();
    //    QTemporaryFile tempFile;
    //    tempFile.open();
    //    QDataStream temp(&tempFile);
    //    temp << (quint32)0xABCDEF;
    //    temp <<*project;
    //    tempFile.flush();
    //    tempFile.seek(0);
    //    qDebug()<<"tempFile:"<<tempFile.size();
    //    QByteArray array = tempFile.readAll();
    //    QByteArray compressedDat = qCompress(array,1);
    //    qDebug()<<"compressedData:"<<compressedDat.size();

    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    out << (quint32)0xABCDEF;
    out << *project;
    QByteArray compressedDat = qCompress(array,1);
    qDebug()<<"compressedData:"<<compressedDat.size();

    file.seek(0);
    file.resize(0);
    file.write(compressedDat);
    file.flush();
    qDebug()<<"compressedFile:"<<file.size();
    setWindowModified(false);
    qDebug("Save the project cost %g ms",time.elapsed());

    return true;
}

bool CDTProjectWidget::saveAsProject(const QString &path)
{    
    if (path.isEmpty())
        return false;

    if (file.isOpen()) file.close();

    file.setFileName(path);
    if (!file.open(QFile::ReadWrite))
        return false;

    emit projectChanged();
    writeProject();
    return true;

}

bool CDTProjectWidget::saveProject(QString &path)
{
    if(file.size()==0)
    {
        return saveAsProject(path);
    }
    else
    {
        return writeProject();
    }
}

void CDTProjectWidget::onOriginalTool(bool toggle)
{
    if (toggle)
    {
        mapCanvas->unsetMapTool(mapCanvas->mapTool());
        mapCanvas->setMapTool(Q_NULLPTR);
    }
}

void CDTProjectWidget::onZoomOutTool(bool toggle)
{
    if (toggle)
    {
        mapCanvas->unsetMapTool(mapCanvas->mapTool());
        mapCanvas->setMapTool(new QgsMapToolZoom(mapCanvas,true));
    }
}

void CDTProjectWidget::onZoomInTool(bool toggle)
{
    if (toggle)
    {        
        mapCanvas->unsetMapTool(mapCanvas->mapTool());
        mapCanvas->setMapTool(new QgsMapToolZoom(mapCanvas,false));
    }
}

void CDTProjectWidget::onPanTool(bool toggle)
{
    if (toggle)
    {        
        mapCanvas->unsetMapTool(mapCanvas->mapTool());
        mapCanvas->setMapTool(new QgsMapToolPan(mapCanvas));
    }
}

void CDTProjectWidget::onFullExtent()
{
    mapCanvas->zoomToFullExtent();
}

void CDTProjectWidget::setLayerVisible(QgsMapLayer *layer, bool visible)
{
    if (layer==Q_NULLPTR) return;

    if (layersVisible.keys().contains(layer))
        layersVisible[layer] = visible;
}

void CDTProjectWidget::appendLayers(QList<QgsMapLayer *> layers)
{    
    activeLayers.append(layers);
    foreach (QgsMapLayer *lyr, activeLayers) {
        layersVisible.insert(lyr,true);
    }
    refreshMapCanvas(false);
}

void CDTProjectWidget::removeLayer(QList<QgsMapLayer *> layer)
{
    foreach (QgsMapLayer *lyr, layer) {
        if (activeLayers.contains(lyr))
        {
            activeLayers.removeAll(lyr);
//            QgsMapLayerRegistry::instance()->removeMapLayer(lyr->id());
        }
    }
    refreshMapCanvas(false);
}

void CDTProjectWidget::refreshMapCanvas(bool zoomToFullExtent)
{
    QList<QgsMapLayer*> mapLayers;

    foreach (QgsMapLayer *lyr, activeLayers) {
        if (lyr->type()==QgsMapLayerType::VectorLayer)
        {
            mapLayers<<lyr;
        }
    }
    foreach (QgsMapLayer *lyr, activeLayers) {
        if (lyr->type()==QgsMapLayerType::RasterLayer)
        {
            mapLayers<<lyr;
        }
    }


    mapCanvas->setLayers(mapLayers);
    if (zoomToFullExtent)
        mapCanvas->zoomToFullExtent();
}

void CDTProjectWidget::onObjectItemChanged(QStandardItem *item)
{
    //layer visible
    CDTProjectTreeItem* treeItem = (CDTProjectTreeItem*)item;
    if (treeItem->mapLayer())
    {
        layersVisible[treeItem->mapLayer()] = treeItem->checkState()==Qt::Checked;
        refreshMapCanvas(false);
    }
}

void CDTProjectWidget::onRenderStarting()
{
    MainWindow *w = MainWindow::getMainWindow();
    if (w)
        w->setEnabled(false);
}

void CDTProjectWidget::onRenderComplete()
{
    MainWindow *w = MainWindow::getMainWindow();
    if (w)
        w->setEnabled(true);
}

QToolBar *CDTProjectWidget::initToolBar()
{
    QToolBar* toolBar = new QToolBar(tr("Navigate"),this);
    toolBar->setIconSize(MainWindow::getIconSize());

    toolButtonOriginal = new QToolButton(this);
    toolButtonOriginal->setText(tr("Original"));
    toolButtonOriginal->setToolTip(toolButtonOriginal->text());
    toolButtonOriginal->setIcon(QIcon(":/Icons/Default.png"));
    toolButtonOriginal->setCheckable(true);
    toolButtonOriginal->setChecked(true);

    toolButtonZoomOut = new QToolButton(this);
    toolButtonZoomOut->setText(tr("Zoom Out"));
    toolButtonZoomOut->setToolTip(toolButtonZoomOut->text());
    toolButtonZoomOut->setIcon(QIcon(":/Icons/ZoomOut.png"));
    toolButtonZoomOut->setCheckable(true);

    toolButtonZoomIn = new QToolButton(this);
    toolButtonZoomIn->setText(tr("Zoom In"));
    toolButtonZoomIn->setToolTip(toolButtonZoomIn->text());
    toolButtonZoomIn->setIcon(QIcon(":/Icons/ZoomIn.png"));
    toolButtonZoomIn->setCheckable(true);

    toolButtonPan = new QToolButton(this);
    toolButtonPan->setText(tr("Pan"));
    toolButtonPan->setToolTip(toolButtonPan->text());
    toolButtonPan->setIcon(QIcon(":/Icons/Pan.png"));
    toolButtonPan->setCheckable(true);

    toolButtonFullExtent = new QToolButton(this);
    toolButtonFullExtent->setText(tr("Full Extent"));
    toolButtonFullExtent->setToolTip(toolButtonFullExtent->text());
    toolButtonFullExtent->setIcon(QIcon(":/Icons/FullExtent.png"));

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(toolButtonOriginal);
    group->addButton(toolButtonZoomOut);
    group->addButton(toolButtonZoomIn);
    group->addButton(toolButtonPan);

    toolBar->addWidget(toolButtonOriginal);
    toolBar->addWidget(toolButtonZoomOut);
    toolBar->addWidget(toolButtonZoomIn );
    toolBar->addWidget(toolButtonPan);
    toolBar->addWidget(toolButtonFullExtent);

    connect(toolButtonOriginal,SIGNAL(toggled(bool)),this,SLOT(onOriginalTool(bool)));
    connect(toolButtonZoomOut,SIGNAL(toggled(bool)),this,SLOT(onZoomOutTool(bool)));
    connect(toolButtonZoomIn ,SIGNAL(toggled(bool)),this,SLOT(onZoomInTool(bool)));
    connect(toolButtonPan,SIGNAL(toggled(bool)),this,SLOT(onPanTool(bool)));
    connect(toolButtonFullExtent,SIGNAL(clicked()),this,SLOT(onFullExtent()));

    return toolBar;
}

bool CDTProjectWidget::openProjectFile(QString filepath)
{
    if (file.isOpen())
        file.close();

    file.setFileName(filepath);
    if (!file.open(QFile::ReadWrite))
    {
        QMessageBox::critical(this,tr("Error"),tr("Open File ")+filepath+tr(" failed!"));
        return false;
    }
    return true;
}

void CDTProjectWidget::createProject(QUuid id)
{
    project = new CDTProjectLayer(id);
    project->setMapCanvas( mapCanvas);


    connect(project,SIGNAL(layerChanged()),this,SIGNAL(projectChanged()));
    connect(project,SIGNAL(appendLayers(QList<QgsMapLayer*>)),this,SLOT(appendLayers(QList<QgsMapLayer*>)));
    connect(project,SIGNAL(removeLayer(QList<QgsMapLayer*>)),this,SLOT(removeLayer(QList<QgsMapLayer*>)));

    treeModelObject->appendRow(project->standardKeyItem());
}

void CDTProjectWidget::untoggledToolBar()
{
    toolButtonZoomOut->setChecked(false);
    toolButtonZoomIn->setChecked(false);
    toolButtonPan->setChecked(false);
}


int CDTProjectWidget::maybeSave()
{
    if(isWindowModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            writeProject();

        }
        return ret;
    }
    return -1;
}

QString CDTProjectWidget::filePath()
{
    return QFileInfo(file).absoluteFilePath();
}

QToolBar *CDTProjectWidget::menuBar()
{
    return (QToolBar *)(this->layout()->menuBar());
}

void CDTProjectWidget::onProjectChanged()
{
    setWindowModified(true);
}


