#include "cdttrainingsampledockwidget.h"
//#include "ui_cdttrainingsampledockwidget.h"
#include "stable.h"
#include "MapTools/cdtmaptoolselecttrainingsamples.h"
#include "cdtimagelayer.h"
#include "mainwindow.h"
#include "cdtprojectwidget.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

CDTTrainingSampleDockWidget::CDTTrainingSampleDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
//    ui(new Ui::CDTSampleAbstractDockWidget),
    groupBoxSamples(new QGroupBox(tr("Edit samples"),this)),
    listView(new QListView(this)),
    toolBar(new QToolBar(this)),
    sampleModel(new QSqlQueryModel(this)),
    lastMapTool(NULL),
    currentMapTool(NULL)
{
    this->setEnabled(false);
    this->setWindowTitle(tr("Training sample sets"));
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(widget);
    vbox->addWidget(groupBoxSamples);
    this->setWidget(widget);

    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename"),this);
    QAction *actionAddNew = new QAction(QIcon(":/Icon/Add.png"),tr("Add a new training sample"),this);
    QAction *actionRemove = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove selected sample"),this);
    toolBar->addActions(QList<QAction*>()<<actionRename<<actionAddNew<<actionRemove);
    toolBar->setIconSize(MainWindow::getIconSize());
    listView->setModel(sampleModel);
    groupBoxSamples->setCheckable(true);
    groupBoxSamples->setChecked(false);    
    QVBoxLayout *groupboxLayout = new QVBoxLayout(groupBoxSamples);
    groupboxLayout->addWidget(toolBar);
    groupboxLayout->addWidget(listView);

    connect(actionRename,SIGNAL(triggered()),SLOT(onActionRename()));
    connect(actionAddNew,SIGNAL(triggered()),SLOT(onActionAdd()));
    connect(actionRemove,SIGNAL(triggered()),SLOT(onActionRemove()));
    connect(groupBoxSamples,SIGNAL(toggled(bool)),SLOT(onGroupBoxToggled(bool)));
    connect(listView,SIGNAL(clicked(QModelIndex)),SLOT(onListViewClicked(QModelIndex)));
    logger()->info("Constructed");
}

CDTTrainingSampleDockWidget::~CDTTrainingSampleDockWidget()
{
//    delete ui;
}

void CDTTrainingSampleDockWidget::updateListView()
{
    sampleModel->setQuery("select name,id from sample_segmentation where segmentationid='"
                          +segmentationID.toString()+"'",QSqlDatabase::database("category"));
    if (sampleModel->rowCount()>0)
        listView->setCurrentIndex(sampleModel->index(0,0));
}

void CDTTrainingSampleDockWidget::setSegmentationID(QUuid uuid)
{
    if (uuid == segmentationID)
    {
        qDebug()<<"Same segmentation ID";
        return;
    }
    segmentationID = uuid;
    updateListView();
}

void CDTTrainingSampleDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{    
    if (layer == NULL)
    {
        this->setEnabled(false);
        return;
    }

    onDockClear();
    CDTSegmentationLayer *segLayer = qobject_cast<CDTSegmentationLayer*>(layer->getAncestor("CDTSegmentationLayer"));
    if (segLayer)
    {
        setSegmentationID(segLayer->id());
        groupBoxSamples->setChecked(false);
        this->setEnabled(true);
        this->setVisible(true);
        this->raise();
        logger()->info("Find the ancestor class CDTSegmentationLayer");
    }
    else
    {
        logger()->info("The ancestor of class CDTSegmentationLayer is not found!");
    }
}

void CDTTrainingSampleDockWidget::onDockClear()
{
    this->setEnabled(false);
    this->setVisible(false);
    segmentationID = QUuid();
    if (sampleModel) sampleModel->clear();
    if (currentMapTool)
    {
        delete currentMapTool;
        currentMapTool = NULL;
    }
}

void CDTTrainingSampleDockWidget::onActionRename()
{
    int index = listView->currentIndex().row();
    if (index<0)
        return;

    QString sampleid   = sampleModel->data(sampleModel->index(index,1)).toString();
    QString sampleName = sampleModel->data(sampleModel->index(index,0)).toString();

    sampleName =
            QInputDialog::getText(this,tr("New Sample Name"),tr("Name:"),QLineEdit::Normal,sampleName);
    if (sampleName.isEmpty())
        return;

    QSqlQuery q(QSqlDatabase::database("category"));
    q.prepare("update sample_segmentation set name = ? where id = ?");
    q.bindValue(0,sampleName);
    q.bindValue(1,sampleid);
    q.exec();

    updateListView();
}

void CDTTrainingSampleDockWidget::onActionAdd()
{
    QString sampleName =
            QInputDialog::getText(this,tr("New training sample name"),tr("Name:"),QLineEdit::Normal,tr("New Sample"));
    if (sampleName.isEmpty())
        return;

    QSqlQuery q(QSqlDatabase::database("category"));
    q.prepare("insert into sample_segmentation values(?,?,?)");
    q.bindValue(0,QUuid::createUuid().toString());
    q.bindValue(1,sampleName);
    q.bindValue(2,segmentationID.toString());
    q.exec();
    qDebug()<<segmentationID;

    updateListView();
}

void CDTTrainingSampleDockWidget::onActionRemove()
{
    int index = listView->currentIndex().row();
    if (index<0)
        return;

    QString sampleid   = sampleModel->data(sampleModel->index(index,1)).toString();
    QSqlQuery q(QSqlDatabase::database("category"));
    q.prepare("delete from sample_segmentation where id = ?");
    q.bindValue(0,sampleid);
    q.exec();

    updateListView();
}

void CDTTrainingSampleDockWidget::onListViewClicked(const QModelIndex &index)
{
    if (currentMapTool)
    {
        QString sampleid   = sampleModel->data(sampleModel->index(index.row(),1)).toString();
        currentMapTool->setSampleID(sampleid);
    }
}

void CDTTrainingSampleDockWidget::onGroupBoxToggled(bool toggled)
{
    toolBar->setEnabled(toggled);
    listView->setEnabled(toggled);
    if (toggled)
    {
        lastMapTool = MainWindow::getCurrentMapCanvas()->mapTool();
        currentMapTool =
                new CDTMapToolSelectTrainingSamples(MainWindow::getCurrentMapCanvas(),!groupBoxSamples->isChecked());
        connect(MainWindow::getCurrentMapCanvas(),SIGNAL(destroyed()),currentMapTool,SLOT(clearRubberBand()));
        MainWindow::getCurrentMapCanvas()->setMapTool(currentMapTool);
        MainWindow::getCurrentProjectWidget()->menuBar()->setEnabled(false);

        int index = listView->currentIndex().row();
        if (index>=0)
        {
            QString sampleid   = sampleModel->data(sampleModel->index(index,1)).toString();
            currentMapTool->setSampleID(sampleid);
        }
    }
    else
    {
        MainWindow::getCurrentMapCanvas()->setMapTool(lastMapTool);
        if(currentMapTool) delete currentMapTool;
        currentMapTool = NULL;
        MainWindow::getCurrentProjectWidget()->menuBar()->setEnabled(true);
    }
}
