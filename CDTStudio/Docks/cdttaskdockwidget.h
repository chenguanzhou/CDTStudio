#ifndef CDTTASKDOCKWIDGET_H
#define CDTTASKDOCKWIDGET_H

#include "cdtdockwidget.h"
#include "log4qt/logger.h"
#include <QMap>

namespace Ui {
class CDTTaskDockWidget;
}
class QStandardItemModel;
class QTableView;

class CDTTaskReply:public QObject
{
    Q_OBJECT    
public:
    explicit CDTTaskReply(QObject *parent = NULL);
    void sendCompleteSignal(QByteArray result);
signals:
    void completed(QByteArray result);
};

class CDTTaskDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
//    Q_ENUMS(Status)
public:
//    enum Status{
//        WAITING,
//        PROCESSING,
//        COMPLETED
//    };
    explicit CDTTaskDockWidget(QWidget *parent = 0);
    ~CDTTaskDockWidget();

public slots:
    CDTTaskReply *appendNewTask(QString id,QString name,QString projectID);
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

    void updateTaskInfo(QString id,int status,QString currentStep,int currentProgress,int totalProgress);
    void onTaskCompleted(QString id,QByteArray result);

private:
    QTableView *tableView;
    QStandardItemModel *model;
    QMap<QUuid,CDTTaskReply*> taskReplies;
};

#endif // CDTTASKDOCKWIDGET_H
