#include "cdtattributedockwidget.h"
#include "ui_cdtattributedockwidget.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"
#include "cdtattributesinterface.h"

extern QList<CDTAttributesInterface *>     attributesPlugins;

CDTAttributeDockWidget::CDTAttributeDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDTAttributeDockWidget),
    _segmentationLayer(NULL)
{
    ui->setupUi(this);

    setWindowTitle(tr("Attributes Manager"));
    setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);

    QMenuBar *menuBar = new QMenuBar(this);
    QAction *actionEditDataSource = new QAction(tr("Edit Data Source"),menuBar);
    connect(actionEditDataSource,SIGNAL(triggered()),this,SLOT(onActionEditDataSourceTriggered()));
    menuBar->addAction(actionEditDataSource);

    QAction *actionGenerateAttributes = new QAction(tr("Generate Attributes"),menuBar);
    connect(actionGenerateAttributes,SIGNAL(triggered()),this,SLOT(onActionGenerateAttributesTriggered()));
    menuBar->addAction(actionGenerateAttributes);

    ui->horizontalLayout->setMenuBar(menuBar);
    connect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),SLOT(onDatabaseChanged(CDTDatabaseConnInfo)));

    QSettings settings("WHU","CDTStudio");
    ui->splitter->restoreGeometry(settings.value("CDTAttributeDockWidget/geometry").toByteArray());
    ui->splitter->restoreState(settings.value("CDTAttributeDockWidget/windowState").toByteArray());
}

CDTAttributeDockWidget::~CDTAttributeDockWidget()
{
    QSettings settings("WHU","CDTStudio");
    settings.setValue("CDTAttributeDockWidget/geometry", ui->splitter->saveGeometry());
    settings.setValue("CDTAttributeDockWidget/windowState", ui->splitter->saveState());
    delete ui;
}

CDTSegmentationLayer *CDTAttributeDockWidget::segmentationLayer() const
{
    return _segmentationLayer;
}

void CDTAttributeDockWidget::setDatabaseURL(CDTDatabaseConnInfo url)
{
    if (_dbConnInfo == url)return;
    _dbConnInfo = url;
    emit databaseURLChanged(url);
}

void CDTAttributeDockWidget::setSegmentationLayer(CDTSegmentationLayer *layer)
{
    if (_segmentationLayer == layer)
        return;

    if (_segmentationLayer)
    {
        disconnect(_segmentationLayer,SIGNAL(destroyed()),this,SLOT(clear()));        
    }

    _segmentationLayer = layer;
    connect(_segmentationLayer,SIGNAL(destroyed()),this,SLOT(clear()));
    setDatabaseURL(_segmentationLayer->databaseURL());
    ui->tabWidget->setEnabled(true);
}

void CDTAttributeDockWidget::updateTable()
{   
    QStringList attributes = attributeNames();
    QStringList originalTables = QSqlDatabase::database("attribute").tables();
    QStringList tableNames;
    foreach (QString name, originalTables) {
        if (attributes.contains(name))
            tableNames<<name;
    }
    foreach (QString tableName, tableNames) {
        QTableView* widget = new QTableView(ui->tabWidget);
        QSqlTableModel* model = new QSqlTableModel(widget,QSqlDatabase::database("attribute"));
        model->setTable(tableName);
        model->select();
        widget->setModel(model);
        widget->resizeColumnsToContents();
        widget->resizeRowsToContents();
        widget->setEditTriggers(QTableView::NoEditTriggers);
        widget->setItemDelegateForColumn(0,new CDTObjectIDDelegate(this));
        widget->verticalHeader()->hide();

        ui->tabWidget->addTab(widget,tableName);
        connect(widget,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
    }
}

void CDTAttributeDockWidget::clear()
{
    ui->tabWidget->setEnabled(false);
    _dbConnInfo = CDTDatabaseConnInfo();
    clearTables();
    _segmentationLayer =NULL;
}

void CDTAttributeDockWidget::onActionEditDataSourceTriggered()
{
    DialogDBConnection dlg(_dbConnInfo);
    if (dlg.exec()==QDialog::Accepted)
    {
        if (dlg.dbConnectInfo() == _dbConnInfo)
            return;
        _segmentationLayer->setDatabaseURL(dlg.dbConnectInfo());
        setDatabaseURL(dlg.dbConnectInfo());
    }
}

void CDTAttributeDockWidget::onActionGenerateAttributesTriggered()
{
    clearTables();
    CDTImageLayer* layer = (CDTImageLayer*)(segmentationLayer()->parent());
    DialogGenerateAttributes dlg(segmentationLayer()->id(),layer->bandCount());
    dlg.exec();
    updateTable();
}

void CDTAttributeDockWidget::onDatabaseChanged(CDTDatabaseConnInfo connInfo)
{
    clearTables();
    if (connInfo.isNull())
        return;

    _dbConnInfo = connInfo;
    QSqlDatabase db = QSqlDatabase::addDatabase(connInfo.dbType,"attribute");
    db.setDatabaseName(connInfo.dbName);
    db.setHostName(connInfo.hostName);
    db.setPort(connInfo.port);

    if (!db.open(connInfo.username, connInfo.password)) {
        QSqlError err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase("attribute");
        QMessageBox::critical(this,tr("Error"),tr("Open database failed!\n information:")+err.text());
    }
    else
    {
        updateTable();
    }
}

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

QStringList CDTAttributeDockWidget::attributeNames()
{
    QStringList list;
    foreach (CDTAttributesInterface *interface, attributesPlugins) {
        list<<interface->tableName();
    }
    return list;
}

void CDTAttributeDockWidget::clearTables()
{
    while (ui->tabWidget->widget(0))
    {
        QWidget *widget = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        delete widget;
    }
}

