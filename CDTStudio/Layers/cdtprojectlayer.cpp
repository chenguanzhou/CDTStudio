#include "cdtprojectlayer.h"

#include "stable.h"
#include "mainwindow.h"
#include "cdtprojecttreeitem.h"
#include "cdtimagelayer.h"
#include "cdtfilesystem.h"
#include"cdttaskdockwidget.h"
#include "dialognewimage.h"
#include "dialogpbcdbinary.h"

CDTProjectLayer::CDTProjectLayer(QUuid uuid, QObject *parent):
    CDTBaseLayer(uuid,parent),
    fileSystem(new CDTFileSystem)
{
    actionAddImage = new QAction(QIcon(":/Icon/Add.png"),tr("Add Image"),this);
    actiobRemoveAllImages = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove All images"),this);
    actionAddPBCDBinary = new QAction(QIcon(":/Icon/2.png"),tr("Add binary CD layer"),this);
    actionAddOBCDBinary = new QAction(QIcon(":/Icon/2.png"),tr("Add binary CD layer"),this);
    actionAddPBCDFromTo = new QAction(QIcon(":/Icon/2p.png"),tr("Add from-to CD layer"),this);
    actionAddOBCDFromTo = new QAction(QIcon(":/Icon/2p.png"),tr("Add from-to CD layer"),this);
    actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename Project"),this);

    keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,CDTProjectTreeItem::GROUP,QString(),this);
    imagesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::GROUP,tr("Images"),this);
    changesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::CHANGE_ROOT,CDTProjectTreeItem::GROUP,tr("Changes"),this);

    keyItem->appendRow(imagesRoot);
    keyItem->appendRow(changesRoot);

    connect(actionAddImage,SIGNAL(triggered()),SLOT(addImageLayer()));
    connect(actiobRemoveAllImages,SIGNAL(triggered()),SLOT(removeAllImageLayers()));
    connect(actionAddPBCDBinary,SIGNAL(triggered()),SLOT(addPBCDBinaryLayer()));
    connect(actionAddOBCDBinary,SIGNAL(triggered()),SLOT(addOBCDBinaryLayer()));
    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
}

CDTProjectLayer::~CDTProjectLayer()
{
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from project where id = '"+uuid.toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    if (fileSystem) delete fileSystem;
}

void CDTProjectLayer::addImageLayer()
{
    DialogNewImage dlg;
    if(dlg.exec() == DialogNewImage::Accepted)
    {
        CDTImageLayer *image = new CDTImageLayer(QUuid::createUuid(),this);
        image->setNameAndPath(dlg.imageName(),dlg.imagePath());
        imagesRoot->appendRow(image->standardKeyItem());
        addImageLayer(image);
    }
}

void CDTProjectLayer::removeImageLayer(CDTImageLayer* image)
{
    int index = images.indexOf(image);
    if (index>=0)
    {
        image->removeAllExtractionLayers();
        image->removeAllSegmentationLayers();
        QStandardItem* item = image->standardKeyItem();
        item->parent()->removeRow(item->index().row());
        images.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<image->canvasLayer());
        delete image;
        emit projectChanged();
    }
}

void CDTProjectLayer::removeAllImageLayers()
{    
    foreach (CDTImageLayer* image, images) {
        removeImageLayer(image);
    }
}

void CDTProjectLayer::addPBCDBinaryLayer()
{
    QUuid prjID = MainWindow::getCurrentProjectID();
    if (isCDEnabled(prjID)==false)
        return;

    CDTTaskReply* reply = DialogPBCDBinary::startBinaryPBCD(prjID);
    connect(reply,SIGNAL(completed(QByteArray)),this,SLOT(addPBCDBinaryLayer(QByteArray)));
}

void CDTProjectLayer::addOBCDBinaryLayer()
{

}

void CDTProjectLayer::addPBCDBinaryLayer(QByteArray result)
{
    QDataStream in(result);
    QList<double> thresholds;
    QString diffPath;
    in>>thresholds>>diffPath;
    qDebug()<<"thresolds: "<<thresholds<<"diff path: "<<diffPath;
}

void CDTProjectLayer::addImageLayer(CDTImageLayer *image)
{    
    images.push_back(image);
    emit projectChanged();
    if (images.size()==1)
        mapCanvas->zoomToFullExtent();
}

void CDTProjectLayer::insertToTable(QString name)
{
    setName(name);
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("insert into project values(?,?)");
    query.bindValue(0,id().toString());
    query.bindValue(1,name);
    query.exec();
}

QString CDTProjectLayer::name() const
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select name from project where id = ?");
    query.bindValue(0,id().toString());
    query.exec();
    query.next();
    return query.value(0).toString();
}

bool CDTProjectLayer::isCDEnabled(QUuid projectID)
{
    if (projectID.isNull())
    {
        QMessageBox::critical(NULL,tr("Warning"),tr("Project ID is null!"));
        return false;
    }

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select * from imagelayer where projectID = ?");
    query.addBindValue(projectID.toString());
    query.exec();

    QStringList imageLayerIDList;
    while(query.next())
    {
        imageLayerIDList<<query.value(0).toString();
    }

    if (imageLayerIDList.count()<2)
    {
        QMessageBox::critical(NULL,tr("Warning"),tr("The count of images in the current project is less than 2!"));
        return false;
    }
    return true;
}

void CDTProjectLayer::setName(const QString &name)
{
    if (this->name() == name)
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE project set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit projectChanged();
}

void CDTProjectLayer::onContextMenuRequest(QWidget* parent)
{    
    QMenu* menu =new QMenu(parent);
    menu->addAction(actionAddImage);
    menu->addAction(actiobRemoveAllImages);
    menu->addSeparator();

    QMenu* menuPBCD =new QMenu(tr("Pixel-based change detection"),parent);
    QMenu* menuOBCD =new QMenu(tr("Object-based change detection"),parent);

    menuPBCD->addAction(actionAddPBCDBinary);
    menuPBCD->addAction(actionAddPBCDFromTo);
    menuOBCD->addAction(actionAddOBCDBinary);
    menuOBCD->addAction(actionAddOBCDFromTo);

    menu->addMenu(menuPBCD);
    menu->addMenu(menuOBCD);

    menu->addSeparator();
    menu->addAction(actionRename);

    menu->exec(QCursor::pos());
}

void CDTProjectLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Input Project Name"),
                                         tr("Project rename:"), QLineEdit::Normal,
                                         name(), &ok);
    if (ok && !text.isEmpty())
    {
        setName(text);
    }
}

QDataStream &operator <<(QDataStream &out,const CDTProjectLayer &project)
{
    out<<project.id()<<project.name()<<*(project.fileSystem);
    out<<project.images.size();
    foreach (CDTImageLayer* image, project.images) {
        out<<*image;
    }


    return out;
}

QDataStream &operator >>(QDataStream &in, CDTProjectLayer &project)
{
    QString name;
    in>>project.uuid>>name>>*(project.fileSystem);
    project.setName(name);

    project.insertToTable(name);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTImageLayer* image = new CDTImageLayer(QUuid(),&project);
        in>>*image;
        project.imagesRoot->appendRow(image->standardKeyItem());
        project.images.push_back(image);
    }

    return in;
}
