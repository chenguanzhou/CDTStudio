#include "cdtvalidationsampledockwidget.h"
#include "stable.h"
#include "mainwindow.h"
#include "cdtimagelayer.h"

CDTValidationSampleDockWidget::CDTValidationSampleDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    groupBox(new QGroupBox(tr("Edit validation sample sets"),this)),
    toolbar(new QToolBar(this)),
    listView(new QListView(this))
{
    this->setEnabled(false);
    this->setWindowTitle(tr("Validation sample sets"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    this->setWidget(widget);
    layout->addWidget(groupBox);

    QVBoxLayout *groupboxLayout = new QVBoxLayout(groupBox);
    groupboxLayout->addWidget(toolbar);
    groupboxLayout->addWidget(listView);
    groupBox->setLayout(groupboxLayout);
    groupBox->setCheckable(true);

    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename"),this);
    QAction *actionAddNew = new QAction(QIcon(":/Icon/Add.png"),tr("Add a sample set"),this);
    QAction *actionRemove = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove"),this);
    toolbar->addActions(QList<QAction*>()<<actionRename<<actionAddNew<<actionRemove);
    toolbar->setIconSize(MainWindow::getIconSize());

}

void CDTValidationSampleDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer==NULL)
        return ;

    onDockClear();
    CDTImageLayer *imageLayer = qobject_cast<CDTImageLayer *>(layer->getAncestor("CDTImageLayer"));
    if (imageLayer == NULL)
    {
        logger()->info("No CDTImageLayer ancestor!");
        return;
    }
    else
    {
        this->setEnabled(true);
        logger()->info("Find CDTImageLayer ancestor");
    }
}

void CDTValidationSampleDockWidget::onDockClear()
{
    this->setEnabled(false);
}
