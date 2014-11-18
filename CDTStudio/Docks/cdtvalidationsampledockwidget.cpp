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

    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename"),this);
    QAction *actionAddNew = new QAction(QIcon(":/Icon/Add.png"),tr("Add a sample set"),this);
    QAction *actionRemove = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove"),this);
    toolBar->addActions(QList<QAction*>()<<actionRename<<actionAddNew<<actionRemove);
    toolBar->setIconSize(MainWindow::getIconSize());

    connect(groupBox,SIGNAL(toggled(bool)),SLOT(onGroupBoxToggled(bool)));
//    connect(listView,SIGNAL(clicked(QModelIndex)),SLOT(onSelectionChanged()));
    connect(actionAddNew,SIGNAL(triggered()),SLOT(onActionAdd()));
    connect(actionRemove,SIGNAL(triggered()),SLOT(onActionRemove()));
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

void CDTValidationSampleDockWidget::onActionAdd()
{
    QStringList info = DialogGenerateValidationSample::
            getGeneratedValidationPointsetName(imageID,this);

    QUuid id = QUuid::createUuid();
    QString name = info[0];
    QString pointsSetName = info[1];
/*  QString name = QInputDialog::getText(this,tr("New validation sample name"),
                                         tr("Name:"),QLineEdit::Normal,tr("New Sample"));
    if (name.isEmpty())
        return;

    for(int i=0;i<sampleModel->rowCount();++i)
    {
        if (sampleModel->data(sampleModel->index(i,0)).toString() == name)
        {
            QMessageBox::critical(this,tr("Error"),tr("The name:'%1' has already exist!").arg(name));
            return;
        }
    }

    QString pointsSetName = QInputDialog::getText(this,tr("Name of new points set"),
                                                  tr("Name:"),QLineEdit::Normal,tr("New points set"));
    if (pointsSetName.isEmpty())
        return;

    bool ok;
    int pointsCount = QInputDialog::getInt(this,tr("Count of validation points"),tr("Points count"),30,1,10000,1,&ok);
    if (ok==false)
        return;

    if (imageID.isNull())
        return;

    CDTImageLayer *imageLayer = CDTImageLayer::getLayer(imageID);
    if (imageLayer==NULL)
        return;

    QgsRectangle extent = imageLayer->canvasLayer()->extent();
    QVector<QPointF> points = generatePoints(pointsCount,extent);
    if (insertPointsIntoDB(points,pointsSetName,id,name)==false)
        return;
    MainWindow::getCurrentProjectWidget()->setWindowModified(true);*/
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

void CDTValidationSampleDockWidget::updateListView()
{
    sampleModel->setQuery(QString("select name,id from image_validation_samples where imageid = '%1'")
                          .arg(imageID.toString()),QSqlDatabase::database("category"));
}

QVector<QPointF> CDTValidationSampleDockWidget::generatePoints(int pointsCount, const QgsRectangle &extent)
{
    const int PRECISE = 10000;
    qsrand(std::clock());

    QVector<QPointF> points;
    for (int i=0;i<pointsCount;++i)
    {
        double x_dot = static_cast<double>(qrand()%PRECISE)/PRECISE;
        double y_dot = static_cast<double>(qrand()%PRECISE)/PRECISE;
        double x = x_dot*extent.width();
        double y = y_dot*extent.height();
        points.push_back(QPointF(x+extent.xMinimum(),y+extent.yMinimum()));
    }
    return points;
}

bool CDTValidationSampleDockWidget::insertPointsIntoDB(
        QVector<QPointF> points,
        const QString &pointsSetName,
        const QString &validationSampleID,
        const QString &validationSampleName)
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);

    CDTProjectLayer *prj = qobject_cast<CDTProjectLayer*>(CDTImageLayer::getLayer(imageID)->parent());

    db.transaction();

    if (query.exec(QString("select id from category where imageid ='%1'").arg(imageID))==false)
    {
        logger()->error("Execute SQL of 'insert into points_project' failed!",imageID);
        db.rollback();
        return false;
    }

    if (query.next() == false)
    {
        QMessageBox::critical(this,tr("Error"),tr("Please set category information first"));
        db.rollback();
        return false;
    }
    QString categoryID = query.value(0).toString();


    if (query.exec(QString("insert into points_project values('%1','%2')")
                   .arg(pointsSetName).arg(prj->id().toString()))==false)
    {
        logger()->error("Execute SQL of 'insert into points_project' failed!");
        db.rollback();
        return false;
    }

    if (query.prepare("insert into points values(?,?,?,?)")==false)
    {
        logger()->error("Prepare SQL failed!");
        db.rollback();
        return false;
    }

    int i=0;
    foreach (QPointF pt, points) {
        query.bindValue(0,i++);
        query.bindValue(1,pt.x());
        query.bindValue(2,pt.y());
        query.bindValue(3,pointsSetName);
        if (query.exec()==false)
        {
            logger()->error("Insert point:(%1,%2) into DB failed. Reason:%3"
                            ,pt.x(),pt.y(),query.lastError().text());
            db.rollback();
            return false;
        }
    }

    if (query.prepare("insert into point_category values(?,?,?)")==false)
    {
        logger()->error("Prepare SQL failed!");
        db.rollback();
        return false;
    }

    i=0;
    foreach (QPointF pt, points) {
        query.bindValue(0,i++);
        query.bindValue(1,categoryID);
        query.bindValue(2,validationSampleID);
        if (query.exec()==false)
        {
            logger()->error("Insert point_category into DB failed. Reason:%3"
                            ,pt.x(),pt.y(),query.lastError().text());
            db.rollback();
            return false;
        }
    }

    if (query.prepare("insert into image_validation_samples values(?,?,?,?)")==false)
    {
        logger()->error("Prepare SQL failed!");
        db.rollback();
        return false;
    }

    query.addBindValue(validationSampleID);
    query.addBindValue(validationSampleName);
    query.addBindValue(imageID.toString());
    query.addBindValue(pointsSetName);
    //    query.addBindValue(dataToVariant(point_category));
    if (query.exec()==false)
    {
        logger()->error("Insert into DB failed. Reason:%1",query.lastError().text());
        db.rollback();
        return false;
    }

    db.commit();
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
    pointsLayer->enableLabels(true);
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
