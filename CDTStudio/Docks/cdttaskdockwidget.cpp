#include "cdttaskdockwidget.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>

CDTTaskReply::CDTTaskReply(QObject *parent)
    :QObject(parent){
}

void CDTTaskReply::sendCompleteSignal(QByteArray result) {
    emit completed(result);
}


CDTTaskDockWidget::CDTTaskDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    tableView(new QTableView(this)),
    model(new QStandardItemModel(this))
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool| Qt::X11BypassWindowManagerHint);
    this->setWindowTitle(tr("Tasks"));
    this->setWidget(tableView);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();

    tableView->resizeRowsToContents();
    logger()->info("Constructed");
}

CDTTaskDockWidget::~CDTTaskDockWidget()
{
    taskReplies.clear();
}

CDTTaskReply* CDTTaskDockWidget::appendNewTask(QString id, QString name, QString projectID)
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
    CDTTaskReply *reply = new CDTTaskReply();
    taskReplies.insert(QUuid(id),reply);
    return reply;
}

void CDTTaskDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    Q_UNUSED(layer);
}

void CDTTaskDockWidget::onDockClear()
{

}

void CDTTaskDockWidget::updateTaskInfo(QString id, int status, QString currentStep, int currentProgress, int totalProgress)
{
    QList<QStandardItem*> lists = model->findItems(id);
    int row = lists[0]->row();
    model->setData(model->index(row,3),status);
    model->setData(model->index(row,4),currentStep);
    model->setData(model->index(row,5),currentProgress);
    model->setData(model->index(row,6),totalProgress);
}

void CDTTaskDockWidget::onTaskCompleted(QString id, QByteArray result)
{
    CDTTaskReply *reply = taskReplies.value(QUuid(id),NULL);
    reply->sendCompleteSignal(result);
//    taskReplies.remove(QUuid(id));
}
