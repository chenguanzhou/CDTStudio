#include "cdtprojectlayer.h"

#include "stable.h"
#include "mainwindow.h"
#include "cdtprojecttreeitem.h"
#include "cdtimagelayer.h"
#include "cdtpixelchangelayer.h"
#include "cdtfilesystem.h"
//#include "cdttaskdockwidget.h"
#include "cdtpbcdbinarylayer.h"
#include "cdtvectorchangelayer.h"
#include "dialognewimage.h"
#include "dialogpbcdbinary.h"
#include "wizardvectorchangedetection.h"
#include "wizardpagepbcddiff.h"
#include "wizardpagepbcdautothreshold.h"

CDTProjectLayer::CDTProjectLayer(QUuid uuid, QObject *parent):
    CDTBaseLayer(uuid,parent),
    fileSystem(new CDTFileSystem)
{

    CDTProjectTreeItem *keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,CDTProjectTreeItem::GROUP,QString(),this);
    imagesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::GROUP,tr("Images"),this);
    pixelChangesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::PIXEL_CHANGE_ROOT,CDTProjectTreeItem::GROUP,tr("Pixel-based Changes"),this);

    vectorChangesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::VECTOR_CHANGE_ROOT,CDTProjectTreeItem::GROUP,tr("Vector-based Changes"),this);

    keyItem->appendRow(imagesRoot);
    keyItem->appendRow(pixelChangesRoot);
    keyItem->appendRow(vectorChangesRoot);
    setKeyItem(keyItem);

    //actions
    QAction *actionAddImage =
            new QAction(QIcon(":/Icons/Add.png"),tr("Add image layer"),this);
    QAction *actiobRemoveAllImages =
            new QAction(QIcon(":/Icons/Remove.png"),tr("Remove all images"),this);
    QAction *actionAddPBCDBinary =
            new QAction(QIcon(":/Icons/2.png"),tr("Add pixel-based change detection(binary) layer"),this);
    QAction *actionAddPBCDFromTo =
            new QAction(QIcon(":/Icons/2p.png"),tr("Add pixel-based change detection(from-to) layer"),this);
    QAction *actiobRemoveAllPixelChanges =
            new QAction(QIcon(":/Icons/Remove.png"),tr("Remove all pixel-based change layers"),this);
    QAction *actionAddVectorCDLayer =
            new QAction(QIcon(":/Icons/OBCD.png"),tr("Add vector-based change detection layer"),this);
    QAction *actiobRemoveAllVectorChanges =
            new QAction(QIcon(":/Icons/Remove.png"),tr("Remove all vector-based change layers"),this);
    QAction *actionRename =
            new QAction(QIcon(":/Icons/Rename.png"),tr("Rename Project"),this);

    setActions(QList<QList<QAction *> >()
               <<(QList<QAction *>()
                  <<actionAddImage
                  <<actiobRemoveAllImages)
               <<(QList<QAction *>()
                  <<actionAddPBCDBinary
                  <<actionAddPBCDFromTo
                  <<actiobRemoveAllPixelChanges)
               <<(QList<QAction *>()
                  <<actionAddVectorCDLayer
                  <<actiobRemoveAllVectorChanges)
               <<(QList<QAction *>()
                  <<actionRename)
    );

    connect(actionAddImage,SIGNAL(triggered()),SLOT(addImageLayer()));
    connect(actiobRemoveAllImages,SIGNAL(triggered()),SLOT(removeAllImageLayers()));
    connect(actionAddPBCDBinary,SIGNAL(triggered()),SLOT(addPBCDBinaryLayer()));
    connect(actiobRemoveAllPixelChanges,SIGNAL(triggered()),SLOT(removeAllPixelChangeLayers()));
    connect(actionAddVectorCDLayer,SIGNAL(triggered()),SLOT(addVectorChangeDetectionLayer()));
    connect(actiobRemoveAllVectorChanges,SIGNAL(triggered()),SLOT(removeAllVectorChangeLayers()));
    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
}

CDTProjectLayer::~CDTProjectLayer()
{
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from project where id = '"+id().toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    ret = query.exec(QString("select pointset_name from points_project where projectid = '%1'").arg(id().toString()));
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
    QStringList list;
    while (query.next()) {
        list<<query.value(0).toString();
    }
    foreach (QString pointset_name, list) {
        ret = query.exec(QString("delete from points where pointset_name = '%1'").arg(pointset_name));
        if (!ret)
            qWarning()<<"prepare:"<<query.lastError().text();
    }

    ret = query.exec(QString("delete from points_project where projectid = '%1'").arg(id().toString()));
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    if (fileSystem) delete fileSystem;
}

void CDTProjectLayer::initLayer(QString name)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare(QString("insert into %1 values(?,?)").arg(tableName()));
    query.bindValue(0,id().toString());
    query.bindValue(1,name);
    query.exec();

    keyItem()->setText(name);
    emit layerChanged();
}

bool CDTProjectLayer::isCDEnabled(QUuid projectID)
{
    if (projectID.isNull())
    {
        QMessageBox::critical(Q_NULLPTR,tr("Warning"),tr("Project ID is null!"));
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
        QMessageBox::critical(Q_NULLPTR,tr("Warning"),tr("The count of images in the current project is less than 2!"));
        return false;
    }
    return true;
}

QString CDTProjectLayer::path() const
{
    return prjPath;
}

void CDTProjectLayer::setPath(QString path)
{
    prjPath = path;
}

void CDTProjectLayer::addImageLayer()
{
    DialogNewImage dlg(this->id());
    if(dlg.exec() == DialogNewImage::Accepted)
    {
        if (dlg.imageName().isEmpty()||dlg.imagePath().isEmpty())
        {
            QMessageBox::critical(Q_NULLPTR,tr("Error"),tr("Image path or name is empty!"));
            return;
        }
        CDTImageLayer *image = new CDTImageLayer(QUuid::createUuid(),this);
        image->initLayer(dlg.imageName(),dlg.imagePath());
        image->setUseRelativePath(dlg.useRelativePath());
        imagesRoot->appendRow(image->standardKeyItem());
        addImageLayer(image);
    }
}

void CDTProjectLayer::addImageLayer(CDTImageLayer *image)
{
    images.push_back(image);
    emit layerChanged();
    if (images.size()==1)
        canvas()->zoomToFullExtent();
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
        emit layerChanged();
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

    QWizard wizard;
    wizard.setWindowTitle(tr("Pixel-based Change Detection Wizard"));
    WizardPagePBCDDiff *page1 = new WizardPagePBCDDiff(prjID);
    WizardPagePBCDAutoThreshold *page2 = new WizardPagePBCDAutoThreshold();

    wizard.addPage(page1);
    wizard.addPage(page2);
    if (wizard.exec()!=QDialog::Accepted)
        return;

    QString resultID = QUuid::createUuid().toString();
    fileSystem->registerFile(resultID,page2->resultImagePath,QString(),QString(),
                             CDTFileSystem::getRasterAffaliated(page2->resultImagePath));

    QVariantMap params;
    params["diffImageID"] = resultID;
    params["positiveThreshold"] = page2->posT();
    params["negetiveThreshold"] = page2->negT();
    CDTPBCDBinaryLayer *layer = new CDTPBCDBinaryLayer(QUuid::createUuid(),this);
    layer->initLayer(
                page1->name(),
                page1->imageID_t1(),
                page1->imageID_t2(),
                params);
    pixelChangesRoot->appendRow(layer->standardKeyItem());
    pixelChanges.push_back(layer);

    emit layerChanged();

//    CDTTaskReply* reply = DialogPBCDBinary::startBinaryPBCD(prjID);
//    connect(reply,SIGNAL(completed(QByteArray)),this,SLOT(addPBCDBinaryLayer(QByteArray)));

}

//void CDTProjectLayer::addPBCDBinaryLayer(QByteArray result)
//{
//    CDTTaskReply *reply = qobject_cast<CDTTaskReply *>(sender());

//    QDataStream in(result);

//    QVariantMap params;
//    in>>params;

//    QString diffImageID = QUuid::createUuid().toString();
//    QString diffPath = params.value("diffPath").toString();
//    fileSystem->registerFile(diffImageID,diffPath,QString(),QString(),
//                             CDTFileSystem::getShapefileAffaliated(diffPath));

//    params.insert("diffImageID",diffImageID);
//    params.remove("diffPath");

//    CDTPBCDBinaryLayer *layer = new CDTPBCDBinaryLayer(QUuid::createUuid(),this);
//    layer->initLayer(
//                reply->property("name").toString(),
//                reply->property("image_t1").toString(),
//                reply->property("image_t2").toString(),
//                params);
//    pixelChangesRoot->appendRow(layer->standardKeyItem());
//    pixelChanges.push_back(layer);

//    emit layerChanged();
//}

void CDTProjectLayer::removePixelChangeLayer(CDTPixelChangeLayer *layer)
{
    int index = pixelChanges.indexOf(layer);
    if (index>=0)
    {
        QStandardItem* item = layer->standardKeyItem();
        item->parent()->removeRow(item->index().row());
        pixelChanges.remove(index);
        foreach (QString fileID, layer->files()) {
            fileSystem->removeFile(fileID);
        }
        emit removeLayer(QList<QgsMapLayer*>()<<layer->canvasLayer());
        delete layer;
        emit layerChanged();
    }
}

void CDTProjectLayer::removeAllPixelChangeLayers()
{
    foreach (CDTPixelChangeLayer* layer, pixelChanges) {
        removePixelChangeLayer(layer);
    }
}

void CDTProjectLayer::addVectorChangeDetectionLayer()
{
    WizardVectorChangeDetection wizard(id());
    if (wizard.exec()==QDialog::Accepted)
    {
        qDebug()<<fileSystem->registerFile( wizard.shapefileID(),wizard.shapefilePath(),QString(),QString()
                                 ,CDTFileSystem::getShapefileAffaliated(wizard.shapefilePath()));
        qDebug()<<wizard.name()<<
                  wizard.shapefilePath()<<
                wizard.shapefileID()<<
                wizard.clsID1()<<
                wizard.clsID2()<<
                wizard.params();
        CDTVectorChangeLayer *layer = new CDTVectorChangeLayer(QUuid::createUuid(),this);
        layer->initLayer(
                    wizard.name(),
                    wizard.shapefileID(),
                    wizard.clsID1(),
                    wizard.clsID2(),
                    wizard.params());
        vectorChangesRoot->appendRow(layer->standardKeyItem());
        addVectorChangeLayer(layer);
    }
}

void CDTProjectLayer::addVectorChangeLayer(CDTVectorChangeLayer *layer)
{
    vectorChanges.push_back(layer);
    emit layerChanged();
}

void CDTProjectLayer::removeVectorChangeLayer(CDTVectorChangeLayer *layer)
{
    int index = vectorChanges.indexOf(layer);
    if (index>=0)
    {
        QStandardItem* item = layer->standardKeyItem();
        item->parent()->removeRow(item->index().row());
        vectorChanges.remove(index);
        fileSystem->removeFile(layer->shapefileID());
        emit removeLayer(QList<QgsMapLayer*>()<<layer->canvasLayer());
        delete layer;
        emit layerChanged();
    }
}

void CDTProjectLayer::removeAllVectorChangeLayers()
{
    foreach (CDTVectorChangeLayer* layer, vectorChanges) {
        removeVectorChangeLayer(layer);
    }
}

QDataStream &operator <<(QDataStream &out,const CDTProjectLayer &project)
{
    out<<project.id()<<project.name()<<*(project.fileSystem);

    //Images
    out<<project.images.size();
    foreach (CDTImageLayer* layer, project.images) {
        out<<*layer;
    }

    //Pixel-based changes
    out<<project.pixelChanges.size();
    foreach (CDTPixelChangeLayer* layer, project.pixelChanges) {
        out<<QString(layer->metaObject()->className())<<*layer;
    }

    //Save table "points"
    QStringList pointsetNames;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select pointset_name from points_project "
                       "where projectid = '%1'").arg(project.id().toString()));
    while (query.next())
    {
        pointsetNames<<query.value(0).toString();
    }

    QMap<QString,QVector<QPair<int,QPointF> > > pointsMap;
    foreach (QString pointSetName, pointsetNames) {
        QVector<QPair<int,QPointF> > points;
        query.exec(QString("select id,x,y from points where pointset_name='%1'").arg(pointSetName));
        while(query.next())
        {
            points<<qMakePair(query.value(0).toInt(),QPointF(query.value(1).toDouble(),query.value(2).toDouble())) ;
        }
        pointsMap.insert(pointSetName,points);
    }
    out<<pointsMap;

    //Vector-based changes
    out<<project.vectorChanges.size();
    foreach (CDTVectorChangeLayer* layer, project.vectorChanges) {
        out<<*layer;
    }
    return out;
}

QDataStream &operator >>(QDataStream &in, CDTProjectLayer &project)
{
    QUuid id;
    QString name;
    in>>id>>name;

    project.setID(id);
    in>>*(project.fileSystem);
//    project.setName(name);

    project.initLayer(name);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTImageLayer* image = new CDTImageLayer(QUuid(),&project);
        in>>*image;
        project.imagesRoot->appendRow(image->standardKeyItem());
        project.images.push_back(image);
    }
    in>>count;
    for (int i=0;i<count;++i)
    {
        QString clsName;
        in>>clsName;
        QMetaObject obj =  CDTPixelChangeLayer::changeLayerMetaObjects.value(clsName);

        CDTPixelChangeLayer *layer = qobject_cast<CDTPixelChangeLayer *>(obj.newInstance(Q_ARG(QUuid,QUuid()),Q_ARG(QObject*,&project)));

        in>>*layer;
        project.pixelChangesRoot->appendRow(layer->standardKeyItem());
        project.pixelChanges.push_back(layer);
    }

    QMap<QString,QVector<QPair<int,QPointF> > > pointsMap;
    in>>pointsMap;
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    foreach (QString pointSetName, pointsMap.keys()) {
        db.transaction();
        bool ret = query.exec(QString("insert into points_project values('%1','%2')")
                              .arg(pointSetName).arg(project.id().toString()));
        if (ret == false)
        {
            QMessageBox::critical(Q_NULLPTR,QObject::tr("Error"),QObject::tr("Insert into points_project failed"));
            db.rollback();
            break;
        }

        QVector<QPair<int,QPointF> > points = pointsMap.value(pointSetName);
        typedef QPair<int,QPointF > PointWithID;
        foreach (PointWithID  pair, points) {
            ret = query.prepare("insert into points values(?,?,?,?)");
            if (ret == false)
            {
                QMessageBox::critical(Q_NULLPTR,QObject::tr("Error"),QObject::tr("Prepare insert into points failed"));
                db.rollback();
                return in;
            }
            query.bindValue(0,pair.first);
            query.bindValue(1,pair.second.x());
            query.bindValue(2,pair.second.y());
            query.bindValue(3,pointSetName);
            ret = query.exec();
            if (ret == false)
            {
                QMessageBox::critical(Q_NULLPTR,QObject::tr("Error"),QObject::tr("Insert into points failed"));
                db.rollback();
                return in;
            }
        }

        db.commit();
    }

    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTVectorChangeLayer* change = new CDTVectorChangeLayer(QUuid(),&project);
        in>>*change;
        project.vectorChangesRoot->appendRow(change->standardKeyItem());
        project.vectorChanges.push_back(change);
    }
    return in;
}
