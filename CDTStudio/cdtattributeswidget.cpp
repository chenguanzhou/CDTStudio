#include "cdtattributeswidget.h"
#include "ui_cdtattributeswidget.h"
#include <QToolBar>
#include <QMenuBar>
#include <QtSql>
#include <QDebug>
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"
#include <QMessageBox>
#include <QTableView>

CDTAttributesWidget::CDTAttributesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDTAttributesWidget),
    _menuBar(new QMenuBar(/*tr("Attributes"),*/this)),
    _segmentationLayer(NULL)
{
    ui->setupUi(this);
    ui->connGroupBox->setEnabled(false);

    QAction *actionEditDataSource = new QAction(tr("Edit Data Source"),_menuBar);
    connect(actionEditDataSource,SIGNAL(triggered()),this,SLOT(onActionEditDataSourceTriggered()));
    _menuBar->addAction(actionEditDataSource);

    QAction *actionGenerateAttributes = new QAction(tr("Generate Attributes"),_menuBar);
    connect(actionGenerateAttributes,SIGNAL(triggered()),this,SLOT(onActionGenerateAttributesTriggered()));
    _menuBar->addAction(actionGenerateAttributes);

    QStringList drivers = QSqlDatabase::drivers();
    // remove compat names
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");

    ui->comboDriver->addItems(drivers);
    int id = ui->comboDriver->findText("QSQLITE");
    if ( id != -1)
        ui->comboDriver->setCurrentIndex(id);

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
    updateWidgetsByUrl(_segmentationLayer->databaseURL());
    connect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),_segmentationLayer,SLOT(setDatabaseURL(CDTDatabaseConnInfo)));
    connect(_segmentationLayer,SIGNAL(destroyed()),this,SLOT(onSegmentationDestroyed()));
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
    ui->connGroupBox->setEnabled(true);
}

void CDTAttributesWidget::onActionGenerateAttributesTriggered()
{
    clearTables();
    DialogGenerateAttributes dlg(segmentationLayer(),3);
    dlg.exec();
    updateTable();
}

void CDTAttributesWidget::on_pushButtonApply_clicked()
{
    ui->connGroupBox->setEnabled(false);
    setDatabaseURL(dbConnInfoFromWidgets()) ;
}
void CDTAttributesWidget::on_pushButtonReset_clicked()
{
    updateWidgetsByUrl(databaseURL());
}

CDTDatabaseConnInfo CDTAttributesWidget::dbConnInfoFromWidgets()
{
    CDTDatabaseConnInfo dbConnInfo;
    dbConnInfo.dbType = ui->comboDriver->currentText();
    dbConnInfo.dbName = ui->editDatabase->text();
    dbConnInfo.username = ui->editUsername->text();
    dbConnInfo.password = ui->editPassword->text();
    dbConnInfo.hostName = ui->editHostname->text();
    dbConnInfo.port = ui->portSpinBox->value();

    return dbConnInfo;
}

void CDTAttributesWidget::updateWidgetsByUrl(const CDTDatabaseConnInfo &dbConnInfo)
{
    if (ui->comboDriver->findText(dbConnInfo.dbType)==-1)
    {
        ui->connGroupBox->setEnabled(false);
        return;
    }
    ui->comboDriver->setCurrentIndex( ui->comboDriver->findText(dbConnInfo.dbType));
    ui->editDatabase->setText(dbConnInfo.dbName);
    ui->editUsername->setText(dbConnInfo.username);
    ui->editPassword->setText(dbConnInfo.password);
    ui->editHostname->setText(dbConnInfo.hostName);
    ui->portSpinBox->setValue(dbConnInfo.port);
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

void CDTAttributesWidget::on_comboDriver_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "QSQLITE")
    {
        ui->editUsername->clear();
        ui->editPassword->clear();
        ui->editHostname->clear();
        ui->portSpinBox->clear();
        ui->editUsername->setEnabled(false);
        ui->editPassword->setEnabled(false);
        ui->editHostname->setEnabled(false);
        ui->portSpinBox->setEnabled(false);
    }
    else
    {
        ui->editUsername->setEnabled(true);
        ui->editPassword->setEnabled(true);
        ui->editHostname->setEnabled(true);
        ui->portSpinBox->setEnabled(true);
    }
}

void CDTAttributesWidget::onDatabaseChanged(CDTDatabaseConnInfo connInfo)
{
    clearTables();

    if (connInfo.isNull())
        return;
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

void CDTAttributesWidget::onSegmentationDestroyed()
{
    ui->tabWidget->setEnabled(false);
    _dbConnInfo = CDTDatabaseConnInfo();
    updateWidgetsByUrl(_dbConnInfo);
}

QDataStream &operator<<(QDataStream &out, const CDTDatabaseConnInfo &dbInfo)
{
    out<<dbInfo.dbType<<dbInfo.dbName<<dbInfo.username<<dbInfo.password<<dbInfo.hostName<<dbInfo.port;
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTDatabaseConnInfo &dbInfo)
{
    in>>dbInfo.dbType>>dbInfo.dbName>>dbInfo.username>>dbInfo.password>>dbInfo.hostName>>dbInfo.port;
    return in;
}

bool CDTDatabaseConnInfo::operator==(const CDTDatabaseConnInfo &rhs) const
{
    return this->dbType==rhs.dbType &&
            this->dbName==rhs.dbName &&
            this->username==rhs.username &&
            this->password==rhs.password &&
            this->hostName==rhs.hostName &&
            this->port==rhs.port;
}

bool CDTDatabaseConnInfo::isNull()
{
    return dbName.isEmpty();
}
