#include "cdtprojectlayer.h"

#include "stable.h"
#include "mainwindow.h"
#include "cdtprojecttreeitem.h"
#include "cdtimagelayer.h"
#include "cdtchangelayer.h"
#include "cdtfilesystem.h"
#include "cdttaskdockwidget.h"
#include "cdtpbcdbinarylayer.h"
#include "dialognewimage.h"
#include "dialogpbcdbinary.h"

CDTProjectLayer::CDTProjectLayer(QUuid uuid, QObject *parent):
    CDTBaseLayer(uuid,parent),
    fileSystem(new CDTFileSystem)
{
    keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,CDTProjectTreeItem::GROUP,QString(),this);
    imagesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::GROUP,tr("Images"),this);
    changesRoot = new CDTProjectTreeItem
            (CDTProjectTreeItem::CHANGE_ROOT,CDTProjectTreeItem::GROUP,tr("Changes"),this);

    keyItem->appendRow(imagesRoot);
    keyItem->appendRow(changesRoot);

    //actions
    QAction *actionAddImage = new QAction(QIcon(":/Icon/Add.png"),tr("Add image layer"),this);
    QAction *actiobRemoveAllImages = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove all images"),this);
    QAction *actionAddPBCDBinary = new QAction(QIcon(":/Icon/2.png"),tr("Add pixel-based change detection(binary) layer"),this);
    QAction *actionAddPBCDFromTo = new QAction(QIcon(":/Icon/2p.png"),tr("Add pixel-based change detection(from-to) layer"),this);
    QAction *actionAddOBCDBinary = new QAction(QIcon(":/Icon/2.png"),tr("Add object-based change detection(binary) layer"),this);
    QAction *actionAddOBCDFromTo = new QAction(QIcon(":/Icon/2p.png"),tr("Add object-based change detection(from-to) layer"),this);
    QAction *actiobRemoveAllChanges = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove all change layers"),this);
    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename Project"),this);

    actions <<(QList<QAction *>()<<actionAddImage<<actiobRemoveAllImages)
           <<(QList<QAction *>()
              <<actionAddPBCDBinary
              <<actionAddPBCDFromTo
              <<actionAddOBCDBinary
              <<actionAddOBCDFromTo
              <<actiobRemoveAllChanges)
          <<(QList<QAction *>()<<actionRename);

    connect(actionAddImage,SIGNAL(triggered()),SLOT(addImageLayer()));
    connect(actiobRemoveAllImages,SIGNAL(triggered()),SLOT(removeAllImageLayers()));
    connect(actionAddPBCDBinary,SIGNAL(triggered()),SLOT(addPBCDBinaryLayer()));
    connect(actionAddOBCDBinary,SIGNAL(triggered()),SLOT(addOBCDBinaryLayer()));
    connect(actiobRemoveAllChanges,SIGNAL(triggered()),SLOT(removeAllChangeLayers()));
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

    ret = query.exec(QString("select pointset_name from points_project where projectid = '%1'").arg(uuid.toString()));
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

    ret = query.exec(QString("delete from points_project where projectid = '%1'").arg(uuid.toString()));
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();

    if (fileSystem) delete fileSystem;
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

void CDTProjectLayer::addImageLayer(CDTImageLayer *image)
{
    images.push_back(image);
    emit layerChanged();
    if (images.size()==1)
        mapCanvas->zoomToFullExtent();
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

void CDTProjectLayer::addPBCDBinaryLayer(QByteArray result)
{
    CDTTaskReply *reply = qobject_cast<CDTTaskReply *>(sender());

    QDataStream in(result);

    QVariantMap params;
    in>>params;
    qDebug()<<"params: "<<params;

    QString diffImageID = QUuid::createUuid().toString();
    QString diffPath = params.value("diffPath").toString();
    fileSystem->registerFile(diffImageID,diffPath,QString(),QString(),
                             CDTFileSystem::getShapefileAffaliated(diffPath));

    params.insert("diffImageID",diffImageID);
    params.remove("diffPath");

    CDTPBCDBinaryLayer *layer = new CDTPBCDBinaryLayer(QUuid::createUuid(),this);
    layer->initLayer(
                reply->property("name").toString(),
                reply->property("image_t1").toString(),
                reply->property("image_t2").toString(),
                params);
    changesRoot->appendRow(layer->standardKeyItem());
    changes.push_back(layer);

    emit layerChanged();
}

void CDTProjectLayer::removeChangeLayer(CDTChangeLayer *layer)
{
    int index = changes.indexOf(layer);
    if (index>=0)
    {
        QStandardItem* item = layer->standardKeyItem();
        item->parent()->removeRow(item->index().row());
        changes.remove(index);
        qDebug()<<"files:"<<layer->files();
        foreach (QString fileID, layer->files()) {
            fileSystem->removeFile(fileID);
        }
        emit removeLayer(QList<QgsMapLayer*>()<<layer->canvasLayer());
        delete layer;
        emit layerChanged();
    }
}

void CDTProjectLayer::removeAllChangeLayers()
{
    foreach (CDTChangeLayer* layer, changes) {
        removeChangeLayer(layer);
    }
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
    emit layerChanged();
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
    foreach (CDTImageLayer* layer, project.images) {
        out<<*layer;
    }

    out<<project.changes.size();
    foreach (CDTChangeLayer* layer, project.changes) {
        out<<QString(layer->metaObject()->className())<<*layer;
    }

    //Save table "points"
    QStringList pointsetNames;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select pointset_name from points_project "
                       "where projectid = '%1'").arg(project.id()));
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
    in>>count;
    for (int i=0;i<count;++i)
    {
        QString clsName;
        in>>clsName;
        QMetaObject obj =  CDTChangeLayer::changeLayerMetaObjects.value(clsName);

        CDTChangeLayer *layer = qobject_cast<CDTChangeLayer *>(obj.newInstance(Q_ARG(QUuid,QUuid()),Q_ARG(QObject*,&project)));

        in>>*layer;
        project.changesRoot->appendRow(layer->standardKeyItem());
        project.changes.push_back(layer);
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
            QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("Insert into points_project failed"));
            db.rollback();
            break;
        }

        QVector<QPair<int,QPointF> > points = pointsMap.value(pointSetName);
        typedef QPair<int,QPointF > PointWithID;
        foreach (PointWithID  pair, points) {
            ret = query.prepare("insert into points values(?,?,?,?)");
            if (ret == false)
            {
                QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("Prepare insert into points failed"));
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
                QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("Insert into points failed"));
                db.rollback();
                return in;
            }
        }

        db.commit();
    }
    return in;
}
