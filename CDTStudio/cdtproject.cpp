#include "cdtproject.h"
#include "dialognewimage.h"
#include <QInputDialog>
#include <QMenu>

CDTProject::CDTProject(QObject *parent):
    CDTBaseObject(parent),
    actionAddImage(new QAction(tr("Add Image"),this)),
    removeAllImages(new QAction(tr("Remove All images"),this)),
    actionRename(new QAction(tr("Rename Project"),this))
{
    keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,CDTProjectTreeItem::GROUP,QString(),this);
    valueItem=new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    connect(actionAddImage,SIGNAL(triggered()),this,SLOT(addImageLayer()));
    connect(removeAllImages,SIGNAL(triggered()),this,SLOT(removeAllImageLayers()));
    connect(actionRename,SIGNAL(triggered()),this,SLOT(onActionRename()));
}

void CDTProject::addImageLayer()
{
    DialogNewImage dlg;
    if(dlg.exec() == DialogNewImage::Accepted)
    {
        CDTImageLayer *image = new CDTImageLayer(this);
        image->setName(dlg.imageName());
        image->setPath(dlg.imagePath());        
        keyItem->appendRow(image->standardItems());
        addImageLayer(image);
    }
}

void CDTProject::removeImageLayer(CDTImageLayer* image)
{
    int index = images.indexOf(image);
    if (index>=0)
    {
        QStandardItem* keyItem = image->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        images.remove(index);        
        delete image;        
        emit projectChanged(this);
    }
}

void CDTProject::removeAllImageLayers()
{
    foreach (CDTImageLayer* image, images) {
        QStandardItem* keyItem = image->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        delete image;
    }
    images.clear();
    emit projectChanged(this);
}

void CDTProject::addImageLayer(CDTImageLayer *image)
{
    images.push_back(image);
    connect(image,SIGNAL(imageLayerChanged()),this,SLOT(childrenChanged()));
    emit projectChanged(this);
}

QString CDTProject::path() const
{
    return projectPath;
}

QString CDTProject::name() const
{
    return projectName;
}

void CDTProject::setName(const QString &n)
{
    projectName = n;
    keyItem->setText(projectName);
}

void CDTProject::setPath(const QString &p)
{
    projectPath = p;
    valueItem->setText(projectPath);
}

void CDTProject::onContextMenuRequest(QWidget* parent)
{
    removeAllImages->setIcon(QIcon(":/Icon/remove.png"));
    actionAddImage->setIcon(QIcon(":/Icon/add.png"));
    actionRename->setIcon(QIcon(":/Icon/rename.png"));
    QMenu* menu =new QMenu(parent);
    menu->addAction(actionAddImage);
    menu->addAction(removeAllImages);
    menu->addSeparator();
    menu->addAction(actionRename);
    menu->exec(QCursor::pos());
}

void CDTProject::onActionRename()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Input Project Name"),
                                         tr("Project name:"), QLineEdit::Normal,
                                         projectName, &ok);
    if (ok && !text.isEmpty())
    {
        setName(text);
        emit projectChanged(this);
    }
}

void CDTProject::childrenChanged()
{
    emit projectChanged(this);
}


QDataStream &operator <<(QDataStream &out,const CDTProject &project)
{
    out<<project.images.size();
    foreach (CDTImageLayer* image, project.images) {
        out<<*image;
    }
    out<<project.projectName<<project.projectPath;
    return out;
}


QDataStream &operator >>(QDataStream &in, CDTProject &project)
{
    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTImageLayer* image = new CDTImageLayer(&project);
        in>>*image;
        project.keyItem->appendRow(image->standardItems());
        CDTProject::connect(image,SIGNAL(imageLayerChanged()),&project,SLOT(childrenChanged()));
        project.images.push_back(image);
    }

    QString temp;
    in>>temp;
    project.setName(temp);
    in>>temp;
    project.setPath(temp);
    return in;
}
