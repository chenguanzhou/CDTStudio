#include "dialogvalidationpoints.h"
#include "stable.h"
#include "mainwindow.h"
#include "cdtprojectwidget.h"

DialogValidationPoints::DialogValidationPoints(const QString validationID, QWidget *parent) :
    QDialog(parent),
    tableView(new QTableView(this)),
    model(new QSqlRelationalTableModel(this,QSqlDatabase::database("category")))
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    this->setLayout(vbox);
    vbox->addWidget(tableView);

    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select imageid from image_validation_samples where id = '%1'").arg(validationID));
    query.next();
    QString imageID = query.value(0).toString();

    query.exec(QString("create temp table category_temp as "
               "select id,name from category where imageid='%1'").arg(imageID));

    model->setTable("point_category");
    model->setRelation(1,QSqlRelation("category_temp","id","name"));
    model->setFilter(QString("validationid = '%1'")
                     .arg(validationID));
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Category"));
    model->select();
    tableView->setModel(model);
    tableView->hideColumn(2);
    tableView->setItemDelegate(new QSqlRelationalDelegate(this));
    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);
    tableView->resizeRowsToContents();
    tableView->resizeColumnsToContents();

    connect(tableView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(onSelectionChanged(QItemSelection)));
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),MainWindow::getCurrentProjectWidget(),SIGNAL(projectChanged()));

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("DialogValidationPoints");
    this->restoreGeometry(setting.value("geometry").toByteArray());
    setting.endGroup();
}

DialogValidationPoints::~DialogValidationPoints()
{

}

void DialogValidationPoints::onSelectionChanged(const QItemSelection &items)
{
    if (items.size()==0)
        return;
    if (items[0].indexes().size()==0)
        return;

    int row = items[0].indexes()[0].row();
    int id = model->data(model->index(row,0)).toInt();
    QString validationID = model->data(model->index(row,2)).toString();

    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select x,y from points where id = '%1' and pointset_name=(select pointset_name from image_validation_samples where id = '%2')").arg(id).arg(validationID));
    query.next();
    double x = query.value(0).toDouble();
    double y = query.value(1).toDouble();


    QgsMapCanvas* mapCanvas = MainWindow::getCurrentMapCanvas();
    if (!mapCanvas)
        return;

    QgsRectangle r = mapCanvas->extent();
    mapCanvas->setExtent(
                QgsRectangle(
                    x - r.width() / 2.0,  y - r.height() / 2.0,
                    x + r.width() / 2.0, y + r.height() / 2.0
                    )
                );
    mapCanvas->refresh();
}

void DialogValidationPoints::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("drop table category_temp");

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("DialogValidationPoints");
    setting.setValue("geometry",this->saveGeometry());
    setting.endGroup();
}

void DialogValidationPoints::enterEvent(QEvent *)
{
    this->setWindowOpacity(1);
}

void DialogValidationPoints::leaveEvent(QEvent *)
{
    this->setWindowOpacity(0.8);
}
