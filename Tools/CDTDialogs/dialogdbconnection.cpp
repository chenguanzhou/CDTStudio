#include "dialogdbconnection.h"
#include "ui_dialogdbconnection.h"
#include <QtCore>
#include <QtSql>
#include <QMessageBox>

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


DialogDBConnection::DialogDBConnection(CDTDatabaseConnInfo dbInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDBConnection),
    dbConnInfo(dbInfo)
{
    ui->setupUi(this);
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

    initialize();
}

DialogDBConnection::~DialogDBConnection()
{
    delete ui;
}

CDTDatabaseConnInfo DialogDBConnection::dbConnectInfo()
{
    updateConnInfo();
    return dbConnInfo;
}

void DialogDBConnection::initialize()
{
    ui->comboDriver->setCurrentIndex( ui->comboDriver->findText(dbConnInfo.dbType));
    ui->editDatabase->setText(dbConnInfo.dbName);
    ui->editUsername->setText(dbConnInfo.username);
    ui->editPassword->setText(dbConnInfo.password);
    ui->editHostname->setText(dbConnInfo.hostName);
    ui->portSpinBox->setValue(dbConnInfo.port);
}

void DialogDBConnection::updateConnInfo()
{
    dbConnInfo.dbType = ui->comboDriver->currentText();
    dbConnInfo.dbName = ui->editDatabase->text();
    dbConnInfo.username = ui->editUsername->text();
    dbConnInfo.password = ui->editPassword->text();
    dbConnInfo.hostName = ui->editHostname->text();
    dbConnInfo.port = ui->portSpinBox->value();
}

void DialogDBConnection::on_toolButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,tr("Open"),ui->editDatabase->text());
    if (!path.isNull())
    {
        ui->editDatabase->setText(path);
    }
}

void DialogDBConnection::on_comboDriver_currentIndexChanged(const QString &arg1)
{
    bool isSQLite = arg1 == QString("QSQLITE");
    ui->toolButton->setVisible(isSQLite);
    ui->editHostname->setEnabled(!isSQLite);
    ui->editUsername->setEnabled(!isSQLite);
    ui->editPassword->setEnabled(!isSQLite);
    ui->portSpinBox->setEnabled(!isSQLite);
}

void DialogDBConnection::on_pushButtonTest_clicked()
{
    updateConnInfo();
    QSqlDatabase db = QSqlDatabase::addDatabase(dbConnInfo.dbType,"test");
    db.setDatabaseName(dbConnInfo.dbName);
    db.setUserName(dbConnInfo.username);
    db.setPassword(dbConnInfo.password);
    db.setHostName(dbConnInfo.hostName);
    db.setPort(dbConnInfo.port);
    if (db.open()==false)
    {
        QMessageBox::warning(this, QObject::tr("Unable to open database"),
                             QObject::tr("An error occurred while opening the connection: ")
                             + db.lastError().text());
        QSqlDatabase::removeDatabase("test");
        return;
    }
    QMessageBox::information(this,tr("Connection Test"),tr("Test Seccessfully!"));
}
