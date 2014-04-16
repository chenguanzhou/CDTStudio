#include "cdtprojectwidget.h"
#include "cdtprojecttabwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include "cdtbaseobject.h"
#include <QToolBar>
#include <qgsapplication.h>
#include <qgsmaptoolzoom.h>
#include <qgsmaptoolpan.h>
#include <qgsmaplayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>

CDTProjectWidget::CDTProjectWidget(QWidget *parent) :
    QWidget(parent),
    project(NULL),
    treeModel(new CDTProjectTreeModel(this)),
    isChanged(false),
    mapCanvas(new QgsMapCanvas(this))
{
    connect(treeModel,SIGNAL(itemChanged(QStandardItem*)),SLOT(onItemChanged(QStandardItem*)));    
    connect(this,SIGNAL(projectChanged()),this,SLOT(setIsChanged()));

    QVBoxLayout *vbox = new QVBoxLayout(this);
    mapCanvas->enableAntiAliasing(true);
    mapCanvas->setCanvasColor(QColor(255, 255, 255));

    vbox->addWidget(mapCanvas);
    this->setLayout(vbox);

    QToolBar* toolBar = initToolBar();
    if (toolBar != NULL)
        vbox->setMenuBar(toolBar);
}

CDTProjectWidget::~CDTProjectWidget()
{
    if(project) delete project;
    file.close();
}

void CDTProjectWidget::onContextMenu(QPoint pt, QModelIndex index)
{
    CDTProjectTreeItem *item =(CDTProjectTreeItem*)treeModel->itemFromIndex(index);
    if(item ==NULL)
        return;
    int type = item->getType();


    CDTBaseObject* correspondingObject = item->getCorrespondingObject();
    if (correspondingObject)
    {
        correspondingObject->onContextMenuRequest(this);
    }
}

bool CDTProjectWidget::readProject(QString &filepath)
{
    QFileInfo info(filepath);
    if(!info.exists())
    {
        qWarning()<<filepath<<" is not exist!";
        return false;
    }

//    project->setLayerInfo(QString(),filepath);
    if (openProjectFile(filepath)==false)
        return false;

    QDataStream in(&(file));
    quint32 magicNumber;
    in>>  magicNumber;
    if (magicNumber != (quint32)0xABCDEF)
    {
        qWarning()<<"Magic number is not match.It is :"<<magicNumber;
        return false;
    }
    createProject(QUuid());
    in>>*project;
    emit projectChanged();
    isChanged = false;

    refreshMapCanvas();
    return true;
}

bool CDTProjectWidget::writeProject()
{
    file.seek(0);
    QDataStream out(&file);
    out << (quint32)0xABCDEF;
    out<<*project;
    file.flush();
    isChanged = false;
    return true;
}

bool CDTProjectWidget::saveAsProject(QString &path)
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

void CDTProjectWidget::onZoomOutTool(bool toggle)
{
    if (toggle)
    {
        untoggledToolBar();
        mapCanvas->setMapTool(zoomOutTool);
    }
}

void CDTProjectWidget::onZoomInTool(bool toggle)
{
    if (toggle)
    {
        untoggledToolBar();
        mapCanvas->setMapTool(zoomInTool);
    }
}

void CDTProjectWidget::onPanTool(bool toggle)
{
    if (toggle)
    {
        untoggledToolBar();
        mapCanvas->setMapTool(panTool);
    }
}

void CDTProjectWidget::onFullExtent()
{
    mapCanvas->zoomToFullExtent();
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
        int index = activeLayers.indexOf(lyr);
        if (index!=-1)
        {
            activeLayers.removeAt(index);
            QgsMapLayerRegistry::instance()->removeMapLayer(lyr->id());
        }
    }
    refreshMapCanvas(false);
}

void CDTProjectWidget::refreshMapCanvas(bool zoomToFullExtent)
{
    QList<QgsMapCanvasLayer> mapLayers;
    foreach (QgsMapLayer *lyr, activeLayers) {
        if (layersVisible.value(lyr)==true && lyr->type()==QgsMapLayer::VectorLayer)
        {
            mapLayers<<QgsMapCanvasLayer(lyr);
        }
    }
    foreach (QgsMapLayer *lyr, activeLayers) {
        if (layersVisible.value(lyr)==true && lyr->type()==QgsMapLayer::RasterLayer)
        {
            mapLayers<<QgsMapCanvasLayer(lyr);
        }
    }
    mapCanvas->setLayerSet(mapLayers);
    if (zoomToFullExtent)
        mapCanvas->zoomToFullExtent();
}

void CDTProjectWidget::onItemChanged(QStandardItem *item)
{
    CDTProjectTreeItem* treeItem = (CDTProjectTreeItem*)item;
    if (treeItem->mapLayer())
    {
        layersVisible[treeItem->mapLayer()] = treeItem->checkState()==Qt::Checked;
        refreshMapCanvas(false);
    }
}

QToolBar *CDTProjectWidget::initToolBar()
{
    QToolBar* toolBar = new QToolBar(tr("Navigate"),this);
    toolBar->setIconSize(QSize(16,16));

    actionZoomOut  = new QAction(QIcon(":/Icon/mActionZoomOut.svg"),tr("Zoom Out"),this);
    actionZoomIn   = new QAction(QIcon(":/Icon/mActionZoomIn.svg"),tr("Zoom In"),this);
    actionPan      = new QAction(QIcon(":/Icon/mActionPan.svg"),tr("Pan"),this);
    actionFullExtent = new QAction(QIcon(":/Icon/mActionZoomFullExtent.svg"),tr("Full Extent"),this);
    QAction* hehe = new QAction(tr("Hehe"),this);

    actionZoomOut->setCheckable(true);
    actionZoomIn->setCheckable(true);
    actionPan->setCheckable(true);

    toolBar->addAction(actionZoomOut);
    toolBar->addAction(actionZoomIn );
    toolBar->addAction(actionPan);
    toolBar->addAction(actionFullExtent);
    toolBar->addAction(hehe);

    connect(actionZoomOut,SIGNAL(triggered(bool)),this,SLOT(onZoomOutTool(bool)));
    connect(actionZoomIn ,SIGNAL(triggered(bool)),this,SLOT(onZoomInTool(bool)));
    connect(actionPan,SIGNAL(triggered(bool)),this,SLOT(onPanTool(bool)));
    connect(actionFullExtent,SIGNAL(triggered()),this,SLOT(onFullExtent()));
    connect(hehe,SIGNAL(triggered()),this,SLOT(onHehe()));

    zoomOutTool = new QgsMapToolZoom(mapCanvas,TRUE);
    zoomInTool = new QgsMapToolZoom(mapCanvas,FALSE);
    panTool = new QgsMapToolPan(mapCanvas);

    zoomOutTool->setAction(actionZoomOut);
    zoomInTool->setAction(actionZoomIn);
    panTool->setAction(actionPan);

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
    project = new CDTProject(id);
    project->setMapCanvas( mapCanvas);


    connect(project,SIGNAL(projectChanged()),this,SIGNAL(projectChanged()));
    connect(project,SIGNAL(appendLayers(QList<QgsMapLayer*>)),this,SLOT(appendLayers(QList<QgsMapLayer*>)));
    connect(project,SIGNAL(removeLayer(QList<QgsMapLayer*>)),this,SLOT(removeLayer(QList<QgsMapLayer*>)));

    treeModel->appendRow(project->standardItems());
//    project->setName(name);
}

void CDTProjectWidget::untoggledToolBar()
{
    actionZoomOut->setChecked(false);
    actionZoomIn->setChecked(false);
    actionPan->setChecked(false);
}

void CDTProjectWidget::onHehe()
{

}

int CDTProjectWidget::maybeSave()
{
    if(isChanged)
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

bool CDTProjectWidget::closeProject(CDTProjectTabWidget* parent,const int &index)
{
    int ret = this->maybeSave();
    if(ret == QMessageBox::Cancel  )
    {
        return false;
    }
    else
    {
        parent->removeTab(index);
        return true;
    }
}

QToolBar *CDTProjectWidget::menuBar()
{
    return (QToolBar *)(this->layout()->menuBar());
}

void CDTProjectWidget::setIsChanged()
{
    isChanged = true;
}


