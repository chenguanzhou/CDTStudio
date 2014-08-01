#include "dialogconsole.h"
#include "ui_dialogconsole.h"
#include <QtCore>
#include <QMessageBox>
#include <QStringListModel>

DialogConsole::DialogConsole(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConsole),
    listModel( NULL)
{
    ui->setupUi(this);
    listModel = new QStringListModel(ui->listView);
    ui->listView->setModel(listModel);
    setWindowFlags(Qt::Window);

    connect(ui->pushButtonRefresh,SIGNAL(clicked()),SLOT(updateDatabases()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(QString)),SLOT(onDatabaseChanged(QString)));
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),SLOT(onTableSelected(QModelIndex)));
    connect(ui->pushButtonQuery,SIGNAL(clicked()),SLOT(onQuery()));
    connect(ui->plainTextEditQuery,SIGNAL(textChanged()),SLOT(onQueryTextChanged()));
}

DialogConsole::~DialogConsole()
{
    delete ui;
}

void DialogConsole::updateDatabases()
{
    ui->comboBox->clear();
    ui->comboBox->addItems(QSqlDatabase::connectionNames());
}

void DialogConsole::onTableSelected(const QModelIndex &index)
{
    QString tableName = listModel->data(index,Qt::DisplayRole).toString();
    QSqlTableModel* tableModel = new QSqlTableModel(ui->tableView,db);
    if (ui->tableView->model())
        delete ui->tableView->model();
    ui->tableView->setModel(tableModel);
    tableModel->setTable(tableName);
    tableModel->select();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void DialogConsole::onQuery()
{
    QSqlQuery query(db);
    if (query.exec(ui->plainTextEditQuery->toPlainText())==false)
    {
        QMessageBox::critical(this,tr("Query failed!"),query.lastError().text());
        return;
    }
    QSqlQueryModel* tableModel = new QSqlQueryModel(ui->tableView);
    if (ui->tableView->model())
        delete ui->tableView->model();
    tableModel->setQuery(query);
    ui->tableView->setModel(tableModel);
}

void DialogConsole::onQueryTextChanged()
{
    ui->pushButtonQuery->setEnabled(!ui->plainTextEditQuery->toPlainText().isEmpty());
}

void DialogConsole::onDatabaseChanged(QString connName)
{
    if (connName.isEmpty())
    {
        listModel->setStringList(QStringList());
        return ;
    }
    db = QSqlDatabase::database(connName);
    if (!db.isValid())
    {
        QMessageBox::critical(this,tr("Error"),db.lastError().text());
        return;
    }
    QStringList list = db.tables();
    listModel->setStringList(list);
}
