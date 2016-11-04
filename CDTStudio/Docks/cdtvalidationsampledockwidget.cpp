#include "cdtvalidationsampledockwidget.h"
#include <ctime>
#include <qgslabel.h>
#include <qgslabelattributes.h>
#include "stable.h"
#include "cdtvariantconverter.h"
#include "mainwindow.h"
#include "cdtprojectwidget.h"
#include "cdtprojectlayer.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "dialogvalidationpoints.h"
#include "dialoggeneratevalidationsample.h"

CDTValidationSampleDockWidget::CDTValidationSampleDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    groupBox(new QGroupBox(tr("Edit validation sample sets"),this)),
    toolBar(new QToolBar(this)),
    listView(new QListView(this)),
    sampleModel(new QSqlQueryModel(this)),
    pointsLayer(NULL)
{
    this->setEnabled(false);
    this->setWindowTitle(tr("Validation sample sets"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    this->setWidget(widget);
    layout->addWidget(groupBox);

    QVBoxLayout *groupboxLayout = new QVBoxLayout(groupBox);
    groupboxLayout->addWidget(toolBar);
    groupboxLayout->addWidget(listView);
    groupBox->setLayout(groupboxLayout);
    groupBox->setCheckable(true);
    groupBox->setChecked(false);

    listView->setSelectionMode(QListView::SingleSelection);
    listView->setSelectionBehavior(QListView::SelectRows);
    listView->setModel(sampleModel);

    QAction *actionRename = new QAction(QIcon(":/Icons/Rename.png"),tr("Rename"),this);
    QAction *actionAddNew = new QAction(QIcon(":/Icons/Add.png"),tr("Add a sample set"),this);
    QAction *actionRemove = new QAction(QIcon(":/Icons/Remove.png"),tr("Remove"),this);
    QAction *actionExport = new QAction(QIcon(":/Icons/Export.png"),tr("Export"),this);
    toolBar->addActions(QList<QAction*>()<<actionRename<<actionAddNew<<actionRemove<<actionExport);
    toolBar->setIconSize(MainWindow::getIconSize());

    connect(groupBox,SIGNAL(toggled(bool)),SLOT(onGroupBoxToggled(bool)));
    connect(actionRename,SIGNAL(triggered()),SLOT(onActionRename()));
    connect(actionAddNew,SIGNAL(triggered()),SLOT(onActionAdd()));
    connect(actionRemove,SIGNAL(triggered()),SLOT(onActionRemove()));
    connect(actionExport,SIGNAL(triggered()),SLOT(onActionExport()));
    logger()->info("Constructed");
}

CDTValidationSampleDockWidget::~CDTValidationSampleDockWidget()
{
    onDockClear();
    logger()->info("Deconstructed");
}

void CDTValidationSampleDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer==NULL)
        return;

    if (layer->id()==imageID)
        return;

    CDTImageLayer *imageLayer = qobject_cast<CDTImageLayer *>(layer->getAncestor("CDTImageLayer"));
    if (imageLayer)
    {
        if (imageLayer->id()==imageID)
            return;
        onDockClear();
        imageID = imageLayer->id();
        this->setEnabled(true);
        this->setVisible(true);
        this->raise();
        logger()->info("Find CDTImageLayer ancestor");
        updateListView();
        this->adjustSize();
    }
    else
        logger()->info("No CDTImageLayer ancestor!");
}

void CDTValidationSampleDockWidget::onDockClear()
{
    this->setEnabled(false);
    this->setVisible(false);
    sampleModel->clear();
    imageID = QUuid();
    clearPointsLayer();
}

void CDTValidationSampleDockWidget::onSelectionChanged(QModelIndex current, QModelIndex)
{
    if (current.isValid()==false)
        return;
    if (current.row()<0)
        return;
    clearPointsLayer();
    createPointsLayer();
}

void CDTValidationSampleDockWidget::onGroupBoxToggled(bool toggled)
{
    toolBar->setEnabled(toggled);
    listView->setEnabled(toggled);
    if (toggled)
    {
        createPointsLayer();
        connect(listView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),SLOT(onSelectionChanged(QModelIndex,QModelIndex)));
    }
    else
    {
        clearPointsLayer();
        disconnect(listView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(onSelectionChanged(QModelIndex,QModelIndex)));
    }
}

void CDTValidationSampleDockWidget::onActionRename()
{
    int row = listView->currentIndex().row()<0;
    if (row)
        return;
    QString oldName = sampleModel->data(sampleModel->index(row,0)).toString();
    QString id = sampleModel->data(sampleModel->index(row,1)).toString();

    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input New Name"),
                tr("Rename:"), QLineEdit::Normal,
                oldName, &ok);
    if (ok && !text.isEmpty() && oldName != text){
        QSqlQuery query(QSqlDatabase::database("category"));
        bool ret = query.prepare(QString("UPDATE image_validation_samples set name = ? where id =?"));
        if (ret==false) return;
        query.bindValue(0,text);
        query.bindValue(1,id);
        ret = query.exec();
        if (ret){
            updateListView();
            MainWindow::getCurrentProjectWidget()->setWindowModified(true);
            QMessageBox::information(this,tr("Completed"),tr("Rename completed!"));
        }
    }

}

void CDTValidationSampleDockWidget::onActionAdd()
{
    QStringList info = DialogGenerateValidationSample::
            getGeneratedValidationPointsetName(imageID,this);

    QUuid id = QUuid::createUuid();
    QString name = info[0];
    QString pointsSetName = info[1];

    insertPointsIntoDB(pointsSetName,id,name);
}

void CDTValidationSampleDockWidget::onActionRemove()
{
    if (QMessageBox::information(this,tr("Remove"),tr("Remove the current validation pointset?")
                                 ,QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
    {
        QString id = sampleModel->data(sampleModel->
                                       index(listView->currentIndex().row(),1)).toString();
        QSqlDatabase db = QSqlDatabase::database("category");
        db.transaction();
        try
        {
            QSqlQuery query(db);
            if (query.exec(QString("delete from point_category where validationid='%1'").arg(id))==false)
                throw std::runtime_error(QString("delete database point_category failed, error:%1")
                                         .arg(query.lastError().text()).toLocal8Bit().constData());
            if (query.exec(QString("delete from image_validation_samples where id='%1'").arg(id))==false)
                throw std::runtime_error(QString("delete database image_validation_samples failed, error:%1")
                                         .arg(query.lastError().text()).toLocal8Bit().constData());
        }
        catch (std::runtime_error e)
        {
            logger()->warn(e.what());
            db.rollback();
            return;
        }
        clearPointsLayer();
        db.commit();
        updateListView();
        MainWindow::getCurrentProjectWidget()->setWindowModified(true);
    }
}

void CDTValidationSampleDockWidget::onActionExport()
{
    QString id = sampleModel->data(sampleModel->
                                   index(listView->currentIndex().row(),1)).toString();
    qDebug()<<id;
    if (id.isEmpty())
        return;

    QString outputPath = QFileDialog::getSaveFileName(this,tr("Export"),QString(),tr("Text file(*.txt)"));
    if (outputPath.isEmpty())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select id,name from segmentationlayer where imageid='%1'").arg(imageID));
    QMap<QString,QString> allSegmentationNames;
    while (query.next()){
        allSegmentationNames.insert(query.value(1).toString(),query.value(0).toString());
    }

    QString selectedSegmentationLayerName =
            QInputDialog::getItem(this,tr("Select a segmentation layer"),tr("Segmentation layer"),allSegmentationNames.keys(),0,false);
    if (selectedSegmentationLayerName.isEmpty())
        return;
    QString segmentationID = allSegmentationNames[selectedSegmentationLayerName];
    CDTSegmentationLayer *segLayer = CDTSegmentationLayer::getLayer(segmentationID);
    QString markfilePath = segLayer->markfileTempPath();
    GDALDataset *poMarkDS = (GDALDataset *)GDALOpen(markfilePath.toUtf8().constData(),GA_ReadOnly);
    if (poMarkDS==NULL)
        return;
    double transform[6],inverse[6];
    poMarkDS->GetGeoTransform(transform);
    GDALInvGeoTransform(transform,inverse);
    auto band = poMarkDS->GetRasterBand(1);


    QFile data(outputPath);
    if (data.open(QFile::WriteOnly))
    {
        QTextStream out(&data);


        QString sql = QString("select t3.x as x, t3.y as y,t4.name as category "
                              "from image_validation_samples t1,point_category t2,points t3,category t4 "
                              "where t1.id=t2.validationID and t2.id=t3.id and t2.categoryID=t4.id "
                              "and t1.id='%1'").arg(id);
        if (query.exec(sql))
        {
            while(query.next())
            {
                double x = query.value(0).toDouble();
                double y = query.value(1).toDouble();
                QString name = query.value(2).toString();

                double _x,_y;
                int buffer = 0;
                GDALApplyGeoTransform(inverse,x,y,&_x,&_y);
                band->RasterIO(GF_Read,_x+0.5,_y+0.5,1,1,&buffer,1,1,GDT_Int32,0,0);
                out<<QString("%1\t%2").arg(buffer).arg(name)<<endl;
            }
        }
        data.close();
        QMessageBox::information(this,tr("Completed"),tr("Export completed!"));
    }

    GDALClose(poMarkDS);
}

void CDTValidationSampleDockWidget::updateListView()
{
    sampleModel->setQuery(QString("select name,id from image_validation_samples where imageid = '%1'")
                          .arg(imageID.toString()),QSqlDatabase::database("category"));
}

bool CDTValidationSampleDockWidget::insertPointsIntoDB(
        const QString &pointsSetName,
        const QString &validationSampleID,
        const QString &validationSampleName)
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    db.transaction();
    try{
        if (query.exec(QString("select id from category where imageid ='%1'").arg(imageID))==false)
            throw "Execute SQL of 'insert into points_project' failed!";

        if (query.next() == false)
        {
            QMessageBox::critical(this,tr("Error"),tr("Please set category information first"));
            return false;
        }

        QString categoryID = query.value(0).toString();

        if (query.prepare("insert into image_validation_samples values(?,?,?,?)")==false)
            throw "Prepare SQL failed!";

        query.addBindValue(validationSampleID);
        query.addBindValue(validationSampleName);
        query.addBindValue(imageID.toString());
        query.addBindValue(pointsSetName);
        if (query.exec()==false)
            throw QString("Insert into DB failed. Reason:%1").arg(query.lastError().text());

        QList<int> ids;
        if (query.exec(QString("select id from points where pointset_name='%1'").arg(pointsSetName))==false)
            throw QString("Execute SQL of 'select id from points where pointset_name='%1' 'failed!").arg(pointsSetName);
        while (query.next()) {
            ids<<query.value(0).toInt();
        }

        if (query.prepare("insert into point_category values(?,?,?)")==false)
            throw "Prepare SQL failed!";

        foreach (int id, ids) {
            query.bindValue(0,id);
            query.bindValue(1,categoryID);
            query.bindValue(2,validationSampleID);
            if (query.exec()==false)
                throw QString("Insert point_category into DB failed. Reason:%1").arg(query.lastError().text());
        }
        db.commit();
    }
    catch (const QString &e){
        logger()->error(e);
        db.rollback();
        return false;
    }

    updateListView();
    logger()->info("Insert points set %1 into DB succeeded!",pointsSetName);
    return true;
}

void CDTValidationSampleDockWidget::createPointsLayer()
{
    if (sampleModel->rowCount()<=0)
        return;

    if (listView->currentIndex().row()<0)
        listView->setCurrentIndex(sampleModel->index(0,0));

    QString validationName = sampleModel->data(listView->currentIndex()).toString();
    QString validationID = sampleModel->data(sampleModel->index(listView->currentIndex().row(),1)).toString();
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select pointset_name from image_validation_samples where name = '%1'")
               .arg(validationName));
    query.next();
    QString pointset_name = query.value(0).toString();
    //    QMap<int,QString> point_category = variantToData<QMap<int,QString> >(query.value(2));

    query.exec(QString("select id,x,y from points where pointset_name = '%1'")
               .arg(pointset_name));


    QString uri = "point?field=id:integer";
    pointsLayer = new QgsVectorLayer(uri,validationName, "memory");
    pointsLayer->addAttribute(QgsField("id",QVariant::Int));
    pointsLayer->enableLabels(true);
    pointsLayer->startEditing();
    pointsLayer->beginEditCommand(tr("Add validations to the tampory layer"));
    while (query.next())
    {
        QgsFeature f(pointsLayer->pendingFields());
        f.setGeometry(QgsGeometry::fromPoint(QgsPoint(query.value(1).toDouble(),query.value(2).toDouble())));
        f.setAttribute("id",query.value(0));
        pointsLayer->addFeature(f);
    }
    pointsLayer->endEditCommand();
    pointsLayer->commitChanges();

    //Set label


    QgsLabel *label = pointsLayer->label();
    label->setLabelField(QgsLabel::Text,0);
    QgsLabelAttributes *attributes= label->labelAttributes();
    attributes->setColor(Qt::white);
    attributes->setBufferEnabled(true);
    attributes->setBufferColor(Qt::black);
    attributes->setOffset(10,10,1);

    QgsMapLayerRegistry::instance()->addMapLayer(pointsLayer);
    auto layers = MainWindow::getCurrentMapCanvas()->layers();
    QList<QgsMapCanvasLayer> mapLayers;
    mapLayers<<pointsLayer;
    foreach (QgsMapLayer *layer, layers) {
        mapLayers<<QgsMapCanvasLayer(layer);
    }
    MainWindow::getCurrentMapCanvas()->setLayerSet(mapLayers);
    MainWindow::getCurrentMapCanvas()->refresh();

    //Make a Dialog to set category
    DialogValidationPoints *dlg = new DialogValidationPoints(validationID,this);
    connect(pointsLayer,SIGNAL(destroyed()),dlg,SLOT(close()));
    dlg->setWindowOpacity(0.7);
    dlg->show();
}

void CDTValidationSampleDockWidget::clearPointsLayer()
{
    if (pointsLayer)
    {
        QgsMapLayerRegistry::instance()->removeMapLayer(pointsLayer->id());
        if (MainWindow::getCurrentMapCanvas())
            MainWindow::getCurrentMapCanvas()->refresh();
        pointsLayer = NULL;
    }
}
