#include "cdttaskdockwidget.h"
#include "ui_cdttaskdockwidget.h"
#include <QStandardItemModel>
#include <QTableView>

CDTTaskDockWidget::CDTTaskDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    tableView(new QTableView(this)),
    model(new QStandardItemModel(this))
{
    this->setWidget(tableView);
    tableView->setModel(model);
}

CDTTaskDockWidget::~CDTTaskDockWidget()
{
}

void CDTTaskDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{

}

void CDTTaskDockWidget::onCurrentProjectClosed()
{

}
