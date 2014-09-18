#include "cdttaskdockwidget.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>

CDTTaskDockWidget::CDTTaskDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    tableView(new QTableView(this)),
    model(new QStandardItemModel(this))
{
    this->setWindowTitle(tr("Tasks"));
    this->setWidget(tableView);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();

    appendNewTask("123","pbcd","456");
    tableView->resizeRowsToContents();
}

CDTTaskDockWidget::~CDTTaskDockWidget()
{
}

void CDTTaskDockWidget::appendNewTask(QString id, QString name, QString projectID)
{
    model->appendRow(QList<QStandardItem*>()
                     <<new QStandardItem(id)
                     <<new QStandardItem(name)
                     <<new QStandardItem(projectID)
                     <<new QStandardItem("WAITING")
                     <<new QStandardItem()
                     <<new QStandardItem("0")
                     <<new QStandardItem("0")
                     <<new QStandardItem());
}

void CDTTaskDockWidget::updateTaskInfo(QString id, int status, QString currentStep, int currentProgress, int totalProgress)
{
    QList<QStandardItem*> lists = model->findItems(id);
    qDebug()<<lists.size();
    int row = lists[0]->row();
    model->setData(model->index(row,3),status);
    model->setData(model->index(row,4),currentStep);
    model->setData(model->index(row,5),currentProgress);
    model->setData(model->index(row,6),totalProgress);
}

void CDTTaskDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{

}

void CDTTaskDockWidget::onCurrentProjectClosed()
{

}
