#include "cdtattributedockwidget.h"
#include "ui_cdtattributedockwidget.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"

#include "mainwindow.h"

//extern QList<CDTAttributesInterface *>     attributesPlugins;

CDTAttributeDockWidget::CDTAttributeDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    ui(new Ui::CDTAttributeDockWidget),
    segmentationLayer(NULL)
{
    ui->setupUi(this);

    setWindowTitle(tr("Attributes Manager"));

    //    QToolBar *toolBar = new QToolBar(this);
    //    toolBar->setIconSize(MainWindow::getIconSize());
    //    ui->horizontalLayout->setMenuBar(toolBar);

    //    QAction *actionGenerateAttributes = new QAction(QIcon(":/Icon/AddProperty.png"),tr("Generate Attributes"),toolBar);
    //    connect(actionGenerateAttributes,SIGNAL(triggered()),this,SLOT(onActionGenerateAttributesTriggered()));
    //    toolBar->addAction(actionGenerateAttributes);

    QSettings settings("WHU","CDTStudio");
    ui->splitter->restoreGeometry(settings.value("CDTAttributeDockWidget/geometry").toByteArray());
    ui->splitter->restoreState(settings.value("CDTAttributeDockWidget/windowState").toByteArray());

    //    connect(ui->tabWidget,SIGNAL(currentChanged(int)),SLOT(onCurrentTabChanged(int)));
}

CDTAttributeDockWidget::~CDTAttributeDockWidget()
{
    QSettings settings("WHU","CDTStudio");
    settings.setValue("CDTAttributeDockWidget/geometry", ui->splitter->saveGeometry());
    settings.setValue("CDTAttributeDockWidget/windowState", ui->splitter->saveState());
    delete ui;
}

CDTSegmentationLayer *CDTAttributeDockWidget::segmLayer() const
{
    return segmentationLayer;
}

void CDTAttributeDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (segmentationLayer == layer)
        return;

    //TODO  Process other layer type;

    clear();
    segmentationLayer = qobject_cast<CDTSegmentationLayer *>(layer);
    if (segmentationLayer)
    {
//        setDatabaseURL(segmentationLayer->databaseURL());
        setTableModels(segmentationLayer->tableModels());
        setEnabled(true);
    }
}

void CDTAttributeDockWidget::onCurrentProjectClosed()
{
    clear();
}

//void CDTAttributeDockWidget::setDatabaseURL(CDTDatabaseConnInfo url)
//{
//    if (dbConnInfo == url)return;
//    dbConnInfo = url;
//    clearTables();
//    updateTable();
//    this->setEnabled(true);
//}

//void CDTAttributeDockWidget::updateTable()
//{
//    QSqlDatabase db;
//    db = QSqlDatabase::addDatabase(dbConnInfo.dbType,"attribute");
//    db.setDatabaseName(dbConnInfo.dbName);
//    db.setHostName(dbConnInfo.hostName);
//    db.setPort(dbConnInfo.port);

//    if (!db.open(dbConnInfo.username, dbConnInfo.password)) {
//        QSqlError err = db.lastError();
//        db = QSqlDatabase();
//        QSqlDatabase::removeDatabase("attribute");
//        QMessageBox::critical(this,tr("Error"),tr("Open database failed!\n information:")+err.text());
//        return;
//    }

//    QStringList attributes = attributeNames();
//    QStringList originalTables = db.tables();
//    QStringList tableNames;
//    foreach (QString name, originalTables) {
//        if (attributes.contains(name))
//            tableNames<<name;
//    }
//    foreach (QString tableName, tableNames) {
//        QTableView* widget = new QTableView(ui->tabWidget);
//        QSqlTableModel* model = new QSqlTableModel(widget,db);
//        model->setTable(tableName);
//        model->select();
//        widget->setModel(model);
//        widget->setSelectionMode(QTableView::SingleSelection);
//        widget->setEditTriggers(QTableView::NoEditTriggers);
//        widget->setItemDelegateForColumn(0,new CDTObjectIDDelegate(this));
//        widget->verticalHeader()->hide();

//        ui->tabWidget->addTab(widget,tableName);
//        connect(widget,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
//    }
//}

void CDTAttributeDockWidget::clear()
{
    this->setEnabled(false);
    dbConnInfo = CDTDatabaseConnInfo();
    clearTables();
    QSqlDatabase::removeDatabase("attribute");
    ui->qwtPlot->clear();
    segmentationLayer =NULL;
}

void CDTAttributeDockWidget::setTableModels(QList<QAbstractTableModel *> models)
{
    clearTables();
    foreach (QAbstractTableModel *model, models) {
        QTableView* widget = new QTableView(ui->tabWidget);
        model->setParent(widget);
        widget->setModel(model);
        widget->setSelectionMode(QTableView::SingleSelection);
        widget->setEditTriggers(QTableView::NoEditTriggers);
        widget->setItemDelegateForColumn(0,new CDTObjectIDDelegate(this));
        widget->verticalHeader()->hide();
        widget->resizeColumnsToContents();
        widget->resizeRowsToContents();

        ui->tabWidget->addTab(widget,model->property("name").toString());
        connect(widget,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
    }
}

//void CDTAttributeDockWidget::onActionGenerateAttributesTriggered()
//{
//    clearTables();
//    CDTImageLayer* layer = (CDTImageLayer*)(segmLayer()->parent());
//    DialogGenerateAttributes dlg(segmLayer()->id(),layer->bandCount());
//    dlg.exec();
//    updateTable();
//}

//void CDTAttributeDockWidget::onCurrentTabChanged(int index)
//{
//    if (index<0)
//        return;
//    QTableView* widget = static_cast<QTableView*>(ui->tabWidget->widget(index));
//    QSqlTableModel* model = static_cast<QSqlTableModel*>(widget->model());
////    model->select();
//    widget->resizeColumnsToContents();
//    widget->resizeRowsToContents();
//}

void CDTAttributeDockWidget::onItemClicked(QModelIndex index)
{    
    QTableView *view = (QTableView *)(ui->tabWidget->currentWidget());
    if (view==NULL)return;

    QString tableName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString featureName = view->model()->headerData(index.column(),Qt::Horizontal).toString();

    if (featureName.toLower() == "objectid")
        return;
    ui->qwtPlot->setDatabase(QSqlDatabase::database("attribute"));
    ui->qwtPlot->setTableName(tableName);
    ui->qwtPlot->setFieldName(featureName);

    ui->qwtPlot->replot();
}

//QStringList CDTAttributeDockWidget::attributeNames()
//{
//    QStringList list;
//    foreach (CDTAttributesInterface *interface, attributesPlugins) {
//        list<<interface->tableName();
//    }
//    return list;
//}

void CDTAttributeDockWidget::clearTables()
{
    while (ui->tabWidget->count()>0)
    {
        QWidget *widget = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        delete widget;
    }
}
