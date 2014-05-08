#include "cdtattributeswidget.h"
#include "ui_cdtattributeswidget.h"
#include "stable.h"
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"



CDTAttributesWidget::CDTAttributesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDTAttributesWidget),
    _menuBar(new QMenuBar(/*tr("Attributes"),*/this)),
    _segmentationLayer(NULL)
{
    ui->setupUi(this);

    QAction *actionEditDataSource = new QAction(tr("Edit Data Source"),_menuBar);
    connect(actionEditDataSource,SIGNAL(triggered()),this,SLOT(onActionEditDataSourceTriggered()));
    _menuBar->addAction(actionEditDataSource);

    QAction *actionGenerateAttributes = new QAction(tr("Generate Attributes"),_menuBar);
    connect(actionGenerateAttributes,SIGNAL(triggered()),this,SLOT(onActionGenerateAttributesTriggered()));
    _menuBar->addAction(actionGenerateAttributes);

    connect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),this,SLOT(onDatabaseChanged(CDTDatabaseConnInfo)));
}

CDTAttributesWidget::~CDTAttributesWidget()
{
    delete ui;
}

QMenuBar *CDTAttributesWidget::menuBar() const
{
    return _menuBar;
}

CDTDatabaseConnInfo CDTAttributesWidget::databaseURL() const
{
    return _dbConnInfo;
}

CDTSegmentationLayer *CDTAttributesWidget::segmentationLayer() const
{
    return _segmentationLayer;
}

void CDTAttributesWidget::setDatabaseURL(CDTDatabaseConnInfo url)
{
    if (_dbConnInfo == url)return;
    _dbConnInfo = url;
    emit databaseURLChanged(url);
}

void CDTAttributesWidget::setSegmentationLayer(CDTSegmentationLayer *layer)
{
    if (_segmentationLayer == layer)        
        return;

    _segmentationLayer = layer;
    setDatabaseURL(_segmentationLayer->databaseURL());
    connect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),_segmentationLayer,SLOT(setDatabaseURL(CDTDatabaseConnInfo)));
    connect(_segmentationLayer,SIGNAL(destroyed()),this,SLOT(clear()));
}

void CDTAttributesWidget::updateTable()
{
    QStringList tableNames = QSqlDatabase::database("attribute").tables();
    foreach (QString tableName, tableNames) {
        QTableView* widget = new QTableView(ui->tabWidget);
        QSqlRelationalTableModel* model = new QSqlRelationalTableModel(widget,QSqlDatabase::database("attribute"));
        model->setTable(tableName);
        model->select();
        widget->setModel(model);
        widget->resizeColumnsToContents();
        widget->resizeRowsToContents();
        widget->setEditTriggers(QTableView::NoEditTriggers);
        ui->tabWidget->addTab(widget,tableName);
    }
}

void CDTAttributesWidget::onActionEditDataSourceTriggered()
{
    DialogDBConnection dlg(_dbConnInfo);
    if (dlg.exec()==QDialog::Accepted)
    {
        qDebug()<<dlg.dbConnectInfo().dbName;
        setDatabaseURL(dlg.dbConnectInfo());
    }

}

void CDTAttributesWidget::onActionGenerateAttributesTriggered()
{
    clearTables();
    DialogGenerateAttributes dlg(segmentationLayer()->id(),3);
    dlg.exec();
    updateTable();
}

void CDTAttributesWidget::clearTables()
{
    while (ui->tabWidget->widget(0))
    {
        QWidget *widget = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        delete widget;
    }
}

void CDTAttributesWidget::onDatabaseChanged(CDTDatabaseConnInfo connInfo)
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
        qDebug()<<"Open database "<<connInfo.dbType<<connInfo.dbName<<"suceed";
        updateTable();
    }
}

void CDTAttributesWidget::clear()
{
    ui->tabWidget->setEnabled(false);
    _dbConnInfo = CDTDatabaseConnInfo();
    clearTables();
}

QDataStream &operator<<(QDataStream &out, const CDTDatabaseConnInfo &dbInfo)
{
    out<<dbInfo.dbType<<dbInfo.dbName<<dbInfo.username<<dbInfo.password<<dbInfo.hostName<<dbInfo.port;
    return out;
}
