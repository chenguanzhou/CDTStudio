#include "cdttrainingsampledockwidget.h"
#include "ui_cdttrainingsampledockwidget.h"
#include "stable.h"
#include "MapTools/cdtmaptoolselecttrainingsamples.h"
#include "cdtimagelayer.h"
#include "mainwindow.h"
#include "cdtprojectwidget.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

CDTTrainingSampleDockWidget::CDTTrainingSampleDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    ui(new Ui::CDTSampleAbstractDockWidget),
    sampleModel(new QSqlQueryModel(this)),
    lastMapTool(NULL),
    currentMapTool(NULL)
{
    ui->setupUi(this);

    this->setEnabled(false);
    ui->listView->setModel(sampleModel);

    ui->toolButtonNewSample->setIconSize(MainWindow::getIconSize());
    ui->toolButtonRemoveSelected->setIconSize(MainWindow::getIconSize());
    ui->toolButtonSampleRename->setIconSize(MainWindow::getIconSize());
}

CDTTrainingSampleDockWidget::~CDTTrainingSampleDockWidget()
{
    delete ui;
}

void CDTTrainingSampleDockWidget::updateListView()
{
    sampleModel->setQuery("select name,id from sample_segmentation where segmentationid='"
                          +segmentationID.toString()+"'",QSqlDatabase::database("category"));
    if (sampleModel->rowCount()>0)
        ui->listView->setCurrentIndex(sampleModel->index(0,0));
}

void CDTTrainingSampleDockWidget::clear()
{
    this->setEnabled(false);
    segmentationID = QUuid();
    sampleModel->clear();
    if (currentMapTool)
    {
        delete currentMapTool;
        currentMapTool = NULL;
    }
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

QUuid CDTTrainingSampleDockWidget::currentCategoryID()
{
    //BUG: bug
    return QUuid();
}

void CDTTrainingSampleDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{    
    if (layer == NULL)
    {
        this->setEnabled(false);
        return;
    }

    clear();
    CDTSegmentationLayer *segLayer = qobject_cast<CDTSegmentationLayer*>(layer);
    if (segLayer)
    {
        setSegmentationID(layer->id());
        this->setEnabled(true);
        return;
    }

    CDTClassificationLayer *clsLayer = qobject_cast<CDTClassificationLayer*>(layer);
    if (clsLayer)
    {
        if (clsLayer->parent() == NULL)
            return;
        setSegmentationID(((CDTSegmentationLayer *)(clsLayer->parent()))->id());
        this->setEnabled(true);
        return;
    }
}

void CDTTrainingSampleDockWidget::onCurrentProjectClosed()
{
    clear();
    this->setEnabled(false);
}

//void CDTTrainingSampleDockWidget::on_toolButtonEditSample_toggled(bool checked)
//{
//    if (currentMapTool)
//        currentMapTool->setReadOnly(!checked);

//}

void CDTTrainingSampleDockWidget::on_toolButtonSampleRename_clicked()
{
    int index = ui->listView->currentIndex().row();
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

void CDTTrainingSampleDockWidget::on_toolButtonNewSample_clicked()
{
    QString sampleName =
            QInputDialog::getText(this,tr("New Sample Name"),tr("Name:"),QLineEdit::Normal,tr("New Sample"));
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

void CDTTrainingSampleDockWidget::on_toolButtonRemoveSelected_clicked()
{
    int index = ui->listView->currentIndex().row();
    if (index<0)
        return;

    QString sampleid   = sampleModel->data(sampleModel->index(index,1)).toString();
    QSqlQuery q(QSqlDatabase::database("category"));
    q.prepare("delete from sample_segmentation where id = ?");
    q.bindValue(0,sampleid);
    q.exec();

    updateListView();
}

void CDTTrainingSampleDockWidget::on_listView_clicked(const QModelIndex &index)
{
    if (currentMapTool)
    {
        QString sampleid   = sampleModel->data(sampleModel->index(index.row(),1)).toString();
        currentMapTool->setSampleID(sampleid);
    }
}

void CDTTrainingSampleDockWidget::on_groupBoxSamples_toggled(bool toggled)
{
    ui->frameSample->setEnabled(toggled);
    if (toggled)
    {
        lastMapTool = MainWindow::getCurrentMapCanvas()->mapTool();
        currentMapTool =
                new CDTMapToolSelectTrainingSamples(MainWindow::getCurrentMapCanvas(),!ui->groupBoxSamples->isChecked());
        connect(MainWindow::getCurrentMapCanvas(),SIGNAL(destroyed()),currentMapTool,SLOT(clearRubberBand()));
        MainWindow::getCurrentMapCanvas()->setMapTool(currentMapTool);
        MainWindow::getCurrentProjectWidget()->menuBar()->setEnabled(false);

        int index = ui->listView->currentIndex().row();
        if (index>=0)
        {
            QString sampleid   = sampleModel->data(sampleModel->index(index,1)).toString();
            currentMapTool->setSampleID(sampleid);
        }
//        currentMapTool->setReadOnly(!ui->toolButtonEditSample->isChecked());
    }
    else
    {
        MainWindow::getCurrentMapCanvas()->setMapTool(lastMapTool);
        if(currentMapTool) delete currentMapTool;
        currentMapTool = NULL;
        MainWindow::getCurrentProjectWidget()->menuBar()->setEnabled(true);
    }
}
