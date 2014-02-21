#include "cdtprojectwidget.h"
#include "cdtprojecttabwidget.h"
#include <QFileDialog>

CDTProjectWidget::CDTProjectWidget(QWidget *parent) :
    QWidget(parent),
    project(new CDTProject(this)),
    treeModel(new CDTProjectTreeModel(this))
{
    connect(this,SIGNAL(projectChanged(CDTProject*)),treeModel,SLOT(update(CDTProject*)));

//    project->addImageLayer("image.1","c:/image1.tif");
//    project->addImageLayer("image.2","c:/image2.tif");
}

void CDTProjectWidget::onContextMenu(QPoint pt, QModelIndex index)
{
    CDTProjectTreeItem *item =(CDTProjectTreeItem*)treeModel->itemFromIndex(index);
    int type = item->getType();

    if(type ==CDTProjectTreeItem::PROJECT_ROOT)
    {
        CDTProject *correspondingObject =(CDTProject*) item->getcorrespondingObject();
        correspondingObject->onContextMenu(this);
        emit projectChanged(project);
    }

    else if(type ==CDTProjectTreeItem::IMAGE_ROOT||type ==CDTProjectTreeItem::SEGMENTION_ROOT)
    {
        CDTImageLayer *correspondingObject =(CDTImageLayer*) item->getcorrespondingObject();
        correspondingObject->onContextMenu(this);
        emit projectChanged(project);
    }

    else if(type ==CDTProjectTreeItem::SEGMENTION||type ==CDTProjectTreeItem::CLASSIFICATION_ROOT)
    {
        CDTSegmentationLayer *correspondingObject =(CDTSegmentationLayer*) item->getcorrespondingObject();
        correspondingObject->onContextMenu(this);
        emit projectChanged(project);
    }
}

bool CDTProjectWidget::readProject(QString &filepath)
{
//    QString filepath = QFileDialog::getOpenFileName(this,tr("Open an project file"),QString(),"*.cdtpro");
    setProjectFile(filepath);
    QDataStream in(&(file));
    quint32 magicNumber;
    in>>  magicNumber;
    if (magicNumber != (quint32)0xABCDEF)
        return false;
    in>>*project;
    emit projectChanged(project);
    return true;
}

void CDTProjectWidget::writeProject()
{
    file.seek(0);
    QDataStream out(&file);
    out << (quint32)0xABCDEF;
    out<<*project;
    file.flush();
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


