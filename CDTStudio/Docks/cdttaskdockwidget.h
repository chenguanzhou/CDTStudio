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

public:
    explicit CDTTaskDockWidget(QWidget *parent = 0);
    ~CDTTaskDockWidget();

public slots:
    void appendNewTask();
    void updateTaskInfo();
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();

private:
    QTableView *tableView;
    QStandardItemModel *model;
};

#endif // CDTTASKDOCKWIDGET_H
