#include "cdttrainingsamplesform.h"
#include "ui_cdttrainingsamplesform.h"
#include "cdtmaptoolselecttrainingsamples.h"
#include "cdtimagelayer.h"
#include <QtCore>
#include <QtSql>
#include <QToolBar>
#include <QMessageBox>

CDTTrainingSamplesForm::CDTTrainingSamplesForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDTTrainingSamplesForm),
    model(new QSqlRelationalTableModel(this,QSqlDatabase::database("category"))),
    delegateColor(new CDTCategoryDelegate(this)),
    delegateImageID(new QSqlRelationalDelegate(this))

{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(16,16));
    toolBar->addActions(QList<QAction*>()
                        <<ui->actionEdit<<ui->actionRevert<<ui->actionSubmit);
    toolBar->addSeparator();
    toolBar->addActions(QList<QAction*>()
                        <<ui->actionInsert<<ui->actionRemove<<ui->actionRemove_All);
    ui->verticalLayout->insertWidget(0,toolBar);

    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Class Name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Color"));

    connect(ui->toolButtonRefresh,SIGNAL(clicked()),SLOT(updateComboBox()));
    connect(model,SIGNAL(primeInsert(int,QSqlRecord&)),SLOT(onPrimeInsert(int,QSqlRecord&)));
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),ui->tableView,SLOT(resizeColumnsToContents()));
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),ui->tableView,SLOT(resizeRowsToContents()));
}

CDTTrainingSamplesForm::~CDTTrainingSamplesForm()
{
    delete ui;
}

void CDTTrainingSamplesForm::updateTable()
{
    model->setTable("category");
    model->setFilter("imageID='"+imageLayerID.toString()+"'");
    model->select();

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->setItemDelegateForColumn(2,delegateColor);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(3);
}

void CDTTrainingSamplesForm::updateComboBox()
{
    QSqlQueryModel *model = new QSqlQueryModel(this);
    if (ui->comboBox->model()) delete ui->comboBox->model();
    ui->comboBox->setModel(model);

    model->setQuery("select name from segmentationlayer where imageID = '"+imageLayerID.toString()+"'",
                    QSqlDatabase::database("category"));

}

void CDTTrainingSamplesForm::setImageID(QUuid uuid)
{
    imageLayerID = uuid;    

    QList<CDTImageLayer*> layers = CDTImageLayer::getLayers();
    foreach (CDTImageLayer* layer, layers) {
        if (layer->id()==uuid)
        {
            connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    layer,SIGNAL(imageLayerChanged()));
            connect(model,SIGNAL(beforeInsert(QSqlRecord&)),
                    layer,SIGNAL(imageLayerChanged()));
            connect(model,SIGNAL(beforeDelete(int)),
                    layer,SIGNAL(imageLayerChanged()));
        }
        else
        {
            disconnect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                       layer,SIGNAL(imageLayerChanged()));
            disconnect(model,SIGNAL(beforeInsert(QSqlRecord&)),
                       layer,SIGNAL(imageLayerChanged()));
            disconnect(model,SIGNAL(beforeDelete(int)),
                       layer,SIGNAL(imageLayerChanged()));
        }
    }

    updateComboBox();
    updateTable();
}

bool CDTTrainingSamplesForm::isValid()
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    bool ret = query.exec("select count(*) from imagelayer where id = '"+ imageLayerID.toString() +"'");
    if (!ret)
        qDebug()<<query.lastError().text();

    query.next();
    qDebug()<<query.value(0);
    if (query.value(0).toInt()==1)
        return true;

    updateTable();
    return false;
}

void CDTTrainingSamplesForm::on_actionInsert_triggered()
{
    if (!this->isValid()) return;

    model->insertRow(model->rowCount());
    ui->actionInsert->setEnabled(false);
}

void CDTTrainingSamplesForm::on_actionRemove_triggered()
{
    if (!this->isValid()) return;
    int row = ui->tableView->currentIndex().row();
    if (row <0) return;
    model->removeRow(row);
}

void CDTTrainingSamplesForm::on_actionRemove_All_triggered()
{
    if (!this->isValid()) return;
    model->removeRows(0,model->rowCount());
}

void CDTTrainingSamplesForm::on_actionRevert_triggered()
{
    if (!this->isValid()) return;
    model->revertAll();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void CDTTrainingSamplesForm::on_actionSubmit_triggered()
{
    if (model->submitAll()==false)
        qDebug()<<"Submit failed:"<<model->lastError();
    ui->actionInsert->setEnabled(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void CDTTrainingSamplesForm::onPrimeInsert(int row, QSqlRecord &record)
{
    record.setValue(0,QUuid::createUuid().toString());
    record.setValue(1,tr("New Class"));
    record.setValue(2,QColor(qrand()%255,qrand()%255,qrand()%255));
    record.setValue(3,imageLayerID.toString());
    record.setGenerated(0,true);
    record.setGenerated(1,true);
    record.setGenerated(2,true);
    record.setGenerated(3,true);
}

void CDTTrainingSamplesForm::on_actionEdit_triggered(bool checked)
{
    if (!this->isValid()) return;

    ui->actionInsert->setEnabled(checked);
    ui->actionRemove->setEnabled(checked);
    ui->actionRemove_All->setEnabled(checked);
    ui->actionRevert->setEnabled(checked);
    ui->actionSubmit->setEnabled(checked);
    if (checked)
        ui->tableView->setEditTriggers(QTableView::DoubleClicked|QTableView::AnyKeyPressed);
    else
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
}

QDataStream &operator <<(QDataStream &out,const CategoryInformation &categoryInformation)
{
    out<<categoryInformation.id<<categoryInformation.categoryName<<categoryInformation.color;
    return out;
}

QDataStream &operator >>(QDataStream &in, CategoryInformation &categoryInformation)
{
    in>>categoryInformation.id>>categoryInformation.categoryName>>categoryInformation.color;
    return in;
}


