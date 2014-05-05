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
}

DialogConsole::~DialogConsole()
{
    delete ui;
}

void DialogConsole::updateTableList()
{
    QStringList list = db.tables();
    listModel->setStringList(list);
}

void DialogConsole::on_pushButtonRefresh_clicked()
{
    db = QSqlDatabase::database(ui->comboBox->currentText());
    if (!db.isValid())
    {
        QMessageBox::critical(this,tr("Error"),db.lastError().text());
    }

    updateTableList();
}

void DialogConsole::on_listView_clicked(const QModelIndex &index)
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

void DialogConsole::on_pushButtonQuery_clicked()
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
