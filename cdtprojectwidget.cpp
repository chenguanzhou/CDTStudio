#include "cdtprojectwidget.h"
#include "cdtprojecttabwidget.h"
#include "cdtbaseobject.h"

CDTProjectWidget::CDTProjectWidget(QWidget *parent) :
    QWidget(parent),
    project(new CDTProject(this)),
    treeModel(new CDTProjectTreeModel(this))
{
    connect(this,SIGNAL(projectChanged(CDTProject*)),treeModel,SLOT(update(CDTProject*)));

    project->addImageLayer("image.1","c:/image1.tif");
    project->addImageLayer("image.2","c:/image2.tif");
}

void CDTProjectWidget::onContextMenu(QPoint pt, QModelIndex index)
{
    CDTProjectTreeItem *item =(CDTProjectTreeItem*)treeModel->itemFromIndex(index);
    int type = item->getType();

/*    if(type ==CDTProjectTreeItem::PROJECT_ROOT)
    {
        CDTProject *correspondingObject =(CDTProject*) item->getCorrespondingObject();
        correspondingObject->onContextMenu(this);
        emit projectChanged(project);
    }

    else if(type ==CDTProjectTreeItem::IMAGE_ROOT||type ==CDTProjectTreeItem::SEGMENTION_ROOT)
    {
        CDTImageLayer *correspondingObject =(CDTImageLayer*) item->getCorrespondingObject();
        correspondingObject->onContextMenu(this);
        emit projectChanged(project);
    }

    else if(type ==CDTProjectTreeItem::SEGMENTION||type ==CDTProjectTreeItem::CLASSIFICATION_ROOT)
    {
        CDTSegmentationLayer *correspondingObject =(CDTSegmentationLayer*) item->getCorrespondingObject();
        correspondingObject->onContextMenu(this);
        emit projectChanged(project);
    }*/
    CDTBaseObject* correspondingObject = item->getCorrespondingObject();
    if (item && correspondingObject)
    {
        correspondingObject->onContextMenuRequest(this);
        emit projectChanged(project);
    }
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

