#include "cdtvalidationsampledockwidget.h"
#include "stable.h"
#include "mainwindow.h"

CDTValidationSampleDockWidget::CDTValidationSampleDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    groupBox(new QGroupBox(tr("Edit validation sample sets"),this)),
    listView(new QListView(this)),
    toolbar(new QToolBar(this))
{
    this->setWindowTitle(tr("Validation sample sets"));
    this->setWidget(groupBox);
    QVBoxLayout *vbox = new QVBoxLayout(groupBox);
    vbox->addWidget(toolbar);
    vbox->addWidget(listView);
    groupBox->setLayout(vbox);
    groupBox->setCheckable(true);

    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename"),this);
    QAction *actionAddNew = new QAction(QIcon(":/Icon/Add.png"),tr("Add a sample set"),this);
    QAction *actionRemove = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove"),this);
    toolbar->addActions(QList<QAction*>()<<actionRename<<actionAddNew<<actionRemove);
    toolbar->setIconSize(MainWindow::getIconSize());
}

void CDTValidationSampleDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{

}

void CDTValidationSampleDockWidget::onCurrentProjectClosed()
{

}
