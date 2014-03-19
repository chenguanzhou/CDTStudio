#include "cdtprojectwidget.h"
#include "cdtprojecttabwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include "cdtbaseobject.h"
#include <QToolBar>
#include "qgsapplication.h"
#include "qgsmaptoolzoom.h"
#include "qgsmaptoolpan.h"

CDTProjectWidget::CDTProjectWidget(QWidget *parent) :
    QWidget(parent),
    project(new CDTProject(this)),
    treeModel(new CDTProjectTreeModel(this)),
    isChanged(false),
    mapCanvas(new QgsMapCanvas(this))
{
    connect(this,SIGNAL(projectChanged(CDTProject*)),treeModel,SLOT(update(CDTProject*)));
    connect(this,SIGNAL(projectChanged(CDTProject*)),this,SLOT(setIsChanged()));
    connect(project,SIGNAL(projectChanged(CDTProject*)),this,SIGNAL(projectChanged(CDTProject*)));

    QVBoxLayout *vbox = new QVBoxLayout(this);
    mapCanvas->enableAntiAliasing(true);
    mapCanvas->setCanvasColor(QColor(255, 255, 255));
//    mapCanvas->setLayerSet(layerSet);
    vbox->addWidget(mapCanvas);
    this->setLayout(vbox);

    QToolBar* toolBar = initToolBar();
    if (toolBar != NULL)
        vbox->setMenuBar(toolBar);

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
    if(info.exists())
    {
        setProjectFile(filepath);
        QDataStream in(&(file));
        quint32 magicNumber;
        in>>  magicNumber;
        if (magicNumber != (quint32)0xABCDEF)
            return false;
        in>>*project;
        emit projectChanged(project);
        isChanged = false;
        return true;
    }
    else
    {
        return false;
    }
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
    else
    {
        return saveFile(path);
    }
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

bool CDTProjectWidget::saveFile(QString &filepath)
{
    QFile savefile(filepath);

    if (!savefile.open(QFile::ReadWrite)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(filepath)
                             .arg(savefile.errorString()));
        return false;
    }
    savefile.seek(0);
    QDataStream out(&savefile);
    out << (quint32)0xABCDEF;
    out<<*project;
    savefile.flush();
    isChanged = false;
    return true;
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


QToolBar *CDTProjectWidget::initToolBar()
{
    QToolBar* toolBar = new QToolBar(tr("Navigate"),this);
    toolBar->setIconSize(QSize(24,24));

    actionZoomOut  = new QAction(QIcon(":/Icon/mActionZoomOut.svg"),tr("Zoom Out"),toolBar);
    actionZoomIn   = new QAction(QIcon(":/Icon/mActionZoomIn.svg"),tr("Zoom In"),toolBar);
    actionPan      = new QAction(QIcon(":/Icon/mActionPan.svg"),tr("Pan"),toolBar);
    actionFullExtent = new QAction(QIcon(":/Icon/mActionZoomFullExtent.svg"),tr("Full Extent"),toolBar);

    actionZoomOut->setCheckable(true);
    actionZoomIn->setCheckable(true);
    actionPan->setCheckable(true);

    toolBar->addAction(actionZoomOut);
    toolBar->addAction(actionZoomIn );
    toolBar->addAction(actionPan);
    toolBar->addAction(actionFullExtent);

    connect(actionZoomOut,SIGNAL(triggered(bool)),this,SLOT(onZoomOutTool(bool)));
    connect(actionZoomIn ,SIGNAL(triggered(bool)),this,SLOT(onZoomInTool(bool)));
    connect(actionPan,SIGNAL(triggered(bool)),this,SLOT(onPanTool(bool)));
    connect(actionFullExtent,SIGNAL(triggered()),this,SLOT(onFullExtent()));

    zoomOutTool = new QgsMapToolZoom(mapCanvas,TRUE);
    zoomInTool = new QgsMapToolZoom(mapCanvas,FALSE);
    panTool = new QgsMapToolPan(mapCanvas);

    zoomOutTool->setAction(actionZoomOut);
    zoomInTool->setAction(actionZoomIn);
    panTool->setAction(actionPan);

    return toolBar;
}

void CDTProjectWidget::untoggledToolBar()
{
    actionZoomOut->setChecked(false);
    actionZoomIn->setChecked(false);
    actionPan->setChecked(false);
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
    int a = this->maybeSave();
    if(a == QMessageBox::Cancel  )
    {
        return false;
    }
    else
    {
        parent->removeTab(index);
        return true;
    }
}


CDTProjectWidget::~CDTProjectWidget()
{
    file.close();
}

void CDTProjectWidget::setProjectName(const QString &name)
{
    project->setName(name);
    emit projectChanged(project);

}

void CDTProjectWidget::setProjectPath(const QString &path)
{
    project->setPath(path);
    emit projectChanged(project);
}

void CDTProjectWidget::setProjectFile(const QString &filepath)
{
    file.setFileName(filepath);
    file.open(QIODevice::ReadWrite);

    emit projectChanged(project);
}

void CDTProjectWidget::setIsChanged()
{
    if(!isChanged)
        isChanged = true;
}


