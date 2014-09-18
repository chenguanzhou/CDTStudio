#ifndef CDTTASKDOCKWIDGET_H
#define CDTTASKDOCKWIDGET_H

#include "cdtdockwidget.h"

namespace Ui {
class CDTTaskDockWidget;
}
class QStandardItemModel;
class QTableView;

class CDTTaskDockWidget : public CDTDockWidget
{
    Q_OBJECT
    Q_ENUMS(Status)
public:
//    enum Status{
//        WAITING,
//        PROCESSING,
//        COMPLETED
//    };
    explicit CDTTaskDockWidget(QWidget *parent = 0);
    ~CDTTaskDockWidget();

public slots:
    void appendNewTask(QString id,QString name,QString projectID);
    void updateTaskInfo(QString id,int status,QString currentStep,int currentProgress,int totalProgress);
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();

private:
    QTableView *tableView;
    QStandardItemModel *model;
};

#endif // CDTTASKDOCKWIDGET_H
