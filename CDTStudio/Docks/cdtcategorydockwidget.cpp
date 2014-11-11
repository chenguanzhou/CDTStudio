#include "cdtcategorydockwidget.h"
#include "stable.h"
#include "mainwindow.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

CDTCategoryDockWidget::CDTCategoryDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    tableView(new QTableView(this)),
    //    categoryModel(new QSqlRelationalTableModel(this,QSqlDatabase::database("category"))),
    categoryModel(NULL),
    delegateColor(new CDTCategoryDelegate(this)),
    actionEdit(new QAction(QIcon(":/Icon/Edit.png"),tr("Edit"),this)),
    actionRevert(new QAction(QIcon(":/Icon/Undo.png"),tr("Revert"),this)),
    actionSubmit(new QAction(QIcon(":/Icon/Ok.png"),tr("Submit"),this)),
    actionInsert(new QAction(QIcon(":/Icon/Add.png"),tr("Insert"),this)),
    actionRemove(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove"),this)),
    actionRemove_All(new QAction(QIcon(":/Icon/Remove.png"),tr("Remove All"),this))
{
    //layout
    this->setEnabled(false);
    QWidget *panel = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(panel);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setIconSize(MainWindow::getIconSize());
    vbox->setSpacing(1);
    vbox->addWidget(toolBar);
    vbox->addWidget(tableView);
    this->setWidget(panel);
    this->setWindowTitle(tr("Categories"));


    //tableView
    tableView->setEditTriggers(QTableView::DoubleClicked|QTableView::AnyKeyPressed);
    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);

    //toolbar
    toolBar->addActions(QList<QAction*>()
                        <<actionEdit<<actionRevert<<actionSubmit);
    toolBar->addSeparator();
    toolBar->addActions(QList<QAction*>()
                        <<actionInsert<<actionRemove<<actionRemove_All);
    connect(actionInsert,SIGNAL(triggered()),SLOT(on_actionInsert_triggered()));
    connect(actionRemove,SIGNAL(triggered()),SLOT(on_actionRemove_triggered()));
    connect(actionRemove_All,SIGNAL(triggered()),SLOT(on_actionRemove_All_triggered()));
    connect(actionRevert,SIGNAL(triggered()),SLOT(on_actionRevert_triggered()));
    connect(actionSubmit,SIGNAL(triggered()),SLOT(on_actionSubmit_triggered()));

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    tableView->setSizePolicy(this->sizePolicy());
//    panel->setSizePolicy(this->sizePolicy());

    logger()->info("Constructed");
}

void CDTCategoryDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer == NULL)
    {
        this->setEnabled(false);
        return;
    }

    onDockClear();
    CDTImageLayer *imgLayer = qobject_cast<CDTImageLayer*>(layer->getAncestor("CDTImageLayer"));
    if (imgLayer)
    {
        logger()->info("Find ancestor class of CDTImageLayer");
        this->updateImageID(imgLayer->id());
        this->setEnabled(true);
        this->setVisible(true);
        this->raise();
        this->adjustSize();
    }
}

void CDTCategoryDockWidget::onDockClear()
{
    CDTImageLayer* layer = CDTImageLayer::getLayer(imageLayerID);
    if (layer) disconnect(categoryModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                          layer,SIGNAL(layerChanged()));
    if (layer) disconnect(categoryModel,SIGNAL(beforeInsert(QSqlRecord&)),
                          layer,SIGNAL(layerChanged()));
    if (layer) disconnect(categoryModel,SIGNAL(beforeDelete(int)),
                          layer,SIGNAL(layerChanged()));
    if (categoryModel)
    {
        delete categoryModel;
        categoryModel = NULL;
    }
    tableView->setModel(new QSqlTableModel(this));
    imageLayerID = QUuid();
    this->setEnabled(false);
    this->setVisible(false);
}

void CDTCategoryDockWidget::updateImageID(QUuid id)
{
    if (id==imageLayerID)
        return;

    imageLayerID = id;
    updateTable();
    CDTImageLayer *imgLayer = CDTImageLayer::getLayer(id);
    connect(categoryModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            imgLayer,SIGNAL(layerChanged()));
    connect(categoryModel,SIGNAL(beforeInsert(QSqlRecord&)),
            imgLayer,SIGNAL(layerChanged()));
    connect(categoryModel,SIGNAL(beforeDelete(int)),
            imgLayer,SIGNAL(layerChanged()));

}

void CDTCategoryDockWidget::updateTable()
{
    if (categoryModel == NULL)
    {
        QAbstractItemModel *model = tableView->model();
        if (model) delete model;
        categoryModel = new QSqlTableModel(this,QSqlDatabase::database("category"));
        categoryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Category Name"));
        categoryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Color"));
        connect(categoryModel,SIGNAL(primeInsert(int,QSqlRecord&)),SLOT(onPrimeInsert(int,QSqlRecord&)));
        tableView->setModel(categoryModel);
    }
    categoryModel->setTable("category");
    categoryModel->setFilter("imageID='"+imageLayerID.toString()+"'");
    categoryModel->select();

    //    if (categoryModel->rowCount()>0)
    //        tableView->setCurrentIndex(categoryModel->index(0,0));

    tableView->setItemDelegateForColumn(2,delegateColor);
    tableView->hideColumn(0);
    tableView->hideColumn(3);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
    this->adjustSize();
}

void CDTCategoryDockWidget::on_actionInsert_triggered()
{
    static int indexOffset = 0;
    QSqlRecord record= categoryModel->record();
    record.setValue(0,QUuid::createUuid().toString());
    record.setValue(1,tr("New Class")+QString::number(indexOffset++));
    record.setValue(2,QColor(qrand()%255,qrand()%255,qrand()%255));
    record.setValue(3,imageLayerID.toString());
    categoryModel->insertRecord(categoryModel->rowCount(),record);
}

void CDTCategoryDockWidget::on_actionRemove_triggered()
{
    QModelIndexList list = tableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, list) {
        categoryModel->removeRow(index.row());
    }

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
    //    static int indexOffset = 0;
    //    record.setValue(0,QUuid::createUuid().toString());
    //    record.setValue(1,tr("New Class")+QString::number(indexOffset++));
    //    record.setValue(2,QColor(qrand()%255,qrand()%255,qrand()%255));
    //    record.setValue(3,imageLayerID.toString());
    //    record.setGenerated(0,true);
    //    record.setGenerated(1,true);
    //    record.setGenerated(2,true);
    //    record.setGenerated(3,true);
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

