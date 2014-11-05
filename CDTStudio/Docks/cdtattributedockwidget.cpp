#include "cdtattributedockwidget.h"
//#include "ui_cdtattributedockwidget.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"
#include "cdtplot2ddockwidget.h"

#include "mainwindow.h"

CDTAttributeDockWidget::CDTAttributeDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
//    ui(new Ui::CDTAttributeDockWidget),
    tabWidget(new QTabWidget(this)),
    segmentationLayer(NULL)
{
//    ui->setupUi(this);

    tabWidget->setMovable(true);
    setWidget(tabWidget);
    setWindowTitle(tr("Attributes Manager"));
}

CDTAttributeDockWidget::~CDTAttributeDockWidget()
{
//    delete ui;
}

CDTSegmentationLayer *CDTAttributeDockWidget::segmLayer() const
{
    return segmentationLayer;
}

void CDTAttributeDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (segmentationLayer == layer)
        return;

    //TODO  Process other layer type;

    clear();
    segmentationLayer = qobject_cast<CDTSegmentationLayer *>(layer->getAncestor("CDTSegmentationLayer"));
    if (segmentationLayer)
    {
        logger()->info("Find ancestor class of CDTSegmentationLayer");
        setTableModels(segmentationLayer->tableModels());
        setEnabled(true);
    }
    else
    {
        logger()->info("No ancestor class of CDTSegmentationLayer found");
        return;
    }
}

void CDTAttributeDockWidget::onCurrentProjectClosed()
{
    clear();
}

void CDTAttributeDockWidget::clear()
{
    this->setEnabled(false);
    dbConnInfo = CDTDatabaseConnInfo();
    clearTables();
    QSqlDatabase::removeDatabase("attribute");
    segmentationLayer =NULL;
}

void CDTAttributeDockWidget::setTableModels(QList<QAbstractTableModel *> models)
{
    clearTables();
    foreach (QAbstractTableModel *model, models) {
        QTableView* widget = new QTableView(tabWidget);
        model->setParent(widget);
        widget->setAlternatingRowColors(true);
        widget->setModel(model);
        widget->setSelectionMode(QTableView::SingleSelection);
        widget->setEditTriggers(QTableView::NoEditTriggers);
        widget->setItemDelegateForColumn(0,new CDTObjectIDDelegate(this));
        widget->verticalHeader()->hide();
        widget->resizeColumnsToContents();
        widget->resizeRowsToContents();

        tabWidget->addTab(widget,model->property("name").toString());
        connect(widget,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
    }
}

void CDTAttributeDockWidget::onItemClicked(QModelIndex index)
{    
    QTableView *view = (QTableView *)(tabWidget->currentWidget());
    if (view==NULL)return;

    QString tableName = tabWidget->tabText(tabWidget->currentIndex());
    QString featureName = view->model()->headerData(index.column(),Qt::Horizontal).toString();

    if (featureName.toLower() == "objectid")
        return;

    MainWindow::getPlot2DDockWidget()->setDataSource(QSqlDatabase::database("attribute"),tableName,featureName);
}

void CDTAttributeDockWidget::clearTables()
{
    while (tabWidget->count()>0)
    {
        QWidget *widget = tabWidget->widget(0);
        tabWidget->removeTab(0);
        delete widget;
    }
}
