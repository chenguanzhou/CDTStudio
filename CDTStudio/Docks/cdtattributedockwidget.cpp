#include "cdtattributedockwidget.h"
#include "ui_cdtattributedockwidget.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"
#include "cdtplot2ddockwidget.h"

#include "mainwindow.h"

CDTAttributeDockWidget::CDTAttributeDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    ui(new Ui::CDTAttributeDockWidget),
    segmentationLayer(NULL)
{
    ui->setupUi(this);

    setWindowTitle(tr("Attributes Manager"));

    QSettings settings("WHU","CDTStudio");
//    ui->splitter->restoreGeometry(settings.value("CDTAttributeDockWidget/geometry").toByteArray());
//    ui->splitter->restoreState(settings.value("CDTAttributeDockWidget/windowState").toByteArray());
}

CDTAttributeDockWidget::~CDTAttributeDockWidget()
{
    QSettings settings("WHU","CDTStudio");
//    settings.setValue("CDTAttributeDockWidget/geometry", ui->splitter->saveGeometry());
//    settings.setValue("CDTAttributeDockWidget/windowState", ui->splitter->saveState());
    delete ui;
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
    segmentationLayer = qobject_cast<CDTSegmentationLayer *>(layer);
    if (segmentationLayer)
    {
        setTableModels(segmentationLayer->tableModels());
        setEnabled(true);
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
//    ui->qwtPlot->clear();
    segmentationLayer =NULL;
}

void CDTAttributeDockWidget::setTableModels(QList<QAbstractTableModel *> models)
{
    clearTables();
    foreach (QAbstractTableModel *model, models) {
        QTableView* widget = new QTableView(ui->tabWidget);
        model->setParent(widget);
        widget->setModel(model);
        widget->setSelectionMode(QTableView::SingleSelection);
        widget->setEditTriggers(QTableView::NoEditTriggers);
        widget->setItemDelegateForColumn(0,new CDTObjectIDDelegate(this));
        widget->verticalHeader()->hide();
        widget->resizeColumnsToContents();
        widget->resizeRowsToContents();

        ui->tabWidget->addTab(widget,model->property("name").toString());
        connect(widget,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
    }
}

void CDTAttributeDockWidget::onItemClicked(QModelIndex index)
{    
    QTableView *view = (QTableView *)(ui->tabWidget->currentWidget());
    if (view==NULL)return;

    QString tableName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString featureName = view->model()->headerData(index.column(),Qt::Horizontal).toString();

    if (featureName.toLower() == "objectid")
        return;
//    ui->qwtPlot->setDatabase(QSqlDatabase::database("attribute"));
//    ui->qwtPlot->setTableName(tableName);
//    ui->qwtPlot->setFieldName(featureName);

//    ui->qwtPlot->replot();

    MainWindow::getPlot2DDockWidget()->setDataSource(QSqlDatabase::database("attribute"),tableName,featureName);
}

void CDTAttributeDockWidget::clearTables()
{
    while (ui->tabWidget->count()>0)
    {
        QWidget *widget = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        delete widget;
    }
}
