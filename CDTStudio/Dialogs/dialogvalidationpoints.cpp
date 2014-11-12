#include "dialogvalidationpoints.h"
#include "stable.h"

DialogValidationPoints::DialogValidationPoints(const QString validationID, QWidget *parent) :
    QDialog(parent),
    tableView(new QTableView(this)),
    model(new QSqlRelationalTableModel(this,QSqlDatabase::database("category")))
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    this->setLayout(vbox);
    vbox->addWidget(tableView);


    model->setTable("point_category");
    model->setFilter(QString("validationid = '%1'").arg(validationID));
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setRelation(1,QSqlRelation("category","id","name"));
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Category"));
    model->select();
    tableView->setModel(model);
    tableView->hideColumn(2);
    tableView->setItemDelegate(new QSqlRelationalDelegate(this));
    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);

    connect(tableView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(onSelectionChanged(QItemSelection)));
}

void DialogValidationPoints::onSelectionChanged(const QItemSelection &items)
{
//    int row = items[0].indexes()[0].row();
//    int id = model->data(model->index(row,0)).toInt();
//    QString validationID = model->data(model->index(row,2)).toString();

//    QSqlQuery query(QSqlDatabase::database("category"));
//    query.exec("select x,y from points ");
}
