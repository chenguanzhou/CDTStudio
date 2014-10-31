#include "cdtcategorydockwidget.h"
#include "stable.h"
#include "mainwindow.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

CDTCategoryDockWidget::CDTCategoryDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    tableView(new QTableView(this)),
    categoryModel(new QSqlRelationalTableModel(this,QSqlDatabase::database("category"))),
    delegateColor(new CDTCategoryDelegate(this)),
    actionEdit(new QAction(QIcon(":/Icon/Edit.png"),tr("Edit"),this)),
    actionRevert(new QAction(QIcon(":/Icon/Undo.png"),tr("Revert"),this)),
    actionSubmit(new QAction(QIcon(":/Icon/Ok.png"),tr("Submit"),this)),
    actionInsert(new QAction(QIcon(":/Icon/Add.png"),tr("Insert"),this)),
    actionRemove(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove"),this)),
    actionRemove_All(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove All"),this))
{
    //layout
    QWidget *panel = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(panel);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setIconSize(MainWindow::getIconSize());
    vbox->addWidget(toolBar);
    vbox->addWidget(tableView);
    this->setWidget(panel);
    this->setWindowTitle(tr("Category Panel"));

    //tableView
    tableView->setModel(categoryModel);
    tableView->setEditTriggers(QTableView::NoEditTriggers);
    categoryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Category Name"));
    categoryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Color"));
    connect(categoryModel,SIGNAL(primeInsert(int,QSqlRecord&)),SLOT(onPrimeInsert(int,QSqlRecord&)));

    //toolbar
    toolBar->addActions(QList<QAction*>()
                        <<actionEdit<<actionRevert<<actionSubmit);
    toolBar->addSeparator();
    toolBar->addActions(QList<QAction*>()
                        <<actionInsert<<actionRemove<<actionRemove_All);

}

void CDTCategoryDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer == NULL)
    {
        this->setEnabled(false);
        return;
    }

    CDTImageLayer *imgLayer = qobject_cast<CDTImageLayer*>(layer);
    if (imgLayer)
    {
        updateImageID(imgLayer->id());
        return;
    }

    CDTSegmentationLayer *segLayer =  qobject_cast<CDTSegmentationLayer*>(layer);
    if (segLayer)
    {
        updateImageID(qobject_cast<CDTImageLayer*>(segLayer->parent())->id());
        return;
    }

    CDTClassificationLayer *clsLayer =  qobject_cast<CDTClassificationLayer*>(layer);
    if (clsLayer)
    {
        updateImageID(qobject_cast<CDTImageLayer*>(segLayer->parent()->parent())->id());
        return;
    }
}

void CDTCategoryDockWidget::onCurrentProjectClosed()
{

}

void CDTCategoryDockWidget::updateImageID(QUuid id)
{
    if (id==imageLayerID)
    {
        qDebug()<<"same imageLayerID";
        return;
    }

    imageLayerID = id;
    CDTImageLayer *imgLayer = CDTImageLayer::getLayer(id);
    connect(categoryModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            imgLayer,SIGNAL(layerChanged()));
    connect(categoryModel,SIGNAL(beforeInsert(QSqlRecord&)),
            imgLayer,SIGNAL(layerChanged()));
    connect(categoryModel,SIGNAL(beforeDelete(int)),
            imgLayer,SIGNAL(layerChanged()));

    updateTable();
    this->setEnabled(true);
}

void CDTCategoryDockWidget::updateTable()
{
    categoryModel->setTable("category");
    categoryModel->setFilter("imageID='"+imageLayerID.toString()+"'");
    categoryModel->select();

    if (categoryModel->rowCount()>0)
        tableView->setCurrentIndex(categoryModel->index(0,0));

    tableView->setItemDelegateForColumn(2,delegateColor);
    tableView->hideColumn(0);
    tableView->hideColumn(3);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionInsert_triggered()
{
    categoryModel->insertRow(categoryModel->rowCount());
    actionInsert->setEnabled(false);
}

void CDTCategoryDockWidget::on_actionRemove_triggered()
{
    int row = tableView->currentIndex().row();
    if (row <0) return;
    categoryModel->removeRow(row);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionRemove_All_triggered()
{
    categoryModel->removeRows(0,categoryModel->rowCount());
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionRevert_triggered()
{
    categoryModel->revertAll();
    actionInsert->setEnabled(true);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionSubmit_triggered()
{
    if (categoryModel->submitAll()==false)
        qDebug()<<"Submit failed:"<<categoryModel->lastError();
    actionInsert->setEnabled(true);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::onPrimeInsert(int , QSqlRecord &record)
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

void CDTCategoryDockWidget::on_actionEdit_triggered(bool checked)
{
    actionInsert->setEnabled(checked);
    actionRemove->setEnabled(checked);
    actionRemove_All->setEnabled(checked);
    actionRevert->setEnabled(checked);
    actionSubmit->setEnabled(checked);
    if (checked)
        tableView->setEditTriggers(QTableView::DoubleClicked|QTableView::AnyKeyPressed);
    else
        tableView->setEditTriggers(QTableView::NoEditTriggers);
}

