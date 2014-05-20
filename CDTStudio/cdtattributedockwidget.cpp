#include "cdtattributedockwidget.h"
#include "ui_cdtattributedockwidget.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "dialoggenerateattributes.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_picker_machine.h>
#include <qwt_symbol.h>

CDTAttributeDockWidget::CDTAttributeDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDTAttributeDockWidget),
    histogram(new QwtPlotCurve),
    _segmentationLayer(NULL)
{
    ui->setupUi(this);
    initHistogram();

    setWindowTitle(tr("Attributes Manager"));
    setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);

    QMenuBar *menuBar = new QMenuBar(this);
    QAction *actionEditDataSource = new QAction(tr("Edit Data Source"),menuBar);
    connect(actionEditDataSource,SIGNAL(triggered()),this,SLOT(onActionEditDataSourceTriggered()));
    menuBar->addAction(actionEditDataSource);

    QAction *actionGenerateAttributes = new QAction(tr("Generate Attributes"),menuBar);
    connect(actionGenerateAttributes,SIGNAL(triggered()),this,SLOT(onActionGenerateAttributesTriggered()));
    menuBar->addAction(actionGenerateAttributes);

    ui->horizontalLayout->setMenuBar(menuBar);
    connect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),SLOT(onDatabaseChanged(CDTDatabaseConnInfo)));
}

CDTAttributeDockWidget::~CDTAttributeDockWidget()
{
    delete ui;
    delete histogram;
}

CDTSegmentationLayer *CDTAttributeDockWidget::segmentationLayer() const
{
    return _segmentationLayer;
}

void CDTAttributeDockWidget::initHistogram()
{
    histogram->attach(ui->qwtPlot);
    histogram->setStyle(QwtPlotCurve::Lines);
    histogram->setPen(QColor(255,0,0),2);
    histogram->setBrush(QBrush(QColor(255,0,0,127)));

    QwtPlotPicker *picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                               QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                               ui->qwtPlot->canvas() );
    picker->setStateMachine( new QwtPickerTrackerMachine() );
    picker->setRubberBandPen( QColor( Qt::green ) );
    picker->setRubberBand( QwtPicker::VLineRubberBand );
    picker->setTrackerPen( QColor( Qt::blue ) );

    QwtPlotCanvas *canvas = (QwtPlotCanvas*)(ui->qwtPlot->canvas());
    canvas->setFrameStyle(QwtPlotCanvas::NoFrame);


}

void CDTAttributeDockWidget::setDatabaseURL(CDTDatabaseConnInfo url)
{
    if (_dbConnInfo == url)return;
    _dbConnInfo = url;
    emit databaseURLChanged(url);
}

void CDTAttributeDockWidget::setSegmentationLayer(CDTSegmentationLayer *layer)
{
    if (_segmentationLayer == layer)
        return;

    if (_segmentationLayer)
    {
//        disconnect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),_segmentationLayer,SLOT(setDatabaseURL(CDTDatabaseConnInfo)));
        disconnect(_segmentationLayer,SIGNAL(destroyed()),this,SLOT(clear()));
    }

    _segmentationLayer = layer;
//    connect(this,SIGNAL(databaseURLChanged(CDTDatabaseConnInfo)),_segmentationLayer,SLOT(setDatabaseURL(CDTDatabaseConnInfo)));
    connect(_segmentationLayer,SIGNAL(destroyed()),this,SLOT(clear()));
    setDatabaseURL(_segmentationLayer->databaseURL());
}

void CDTAttributeDockWidget::updateTable()
{
    QStringList tableNames = QSqlDatabase::database("attribute").tables();
    foreach (QString tableName, tableNames) {
        QTableView* widget = new QTableView(ui->tabWidget);
        QSqlRelationalTableModel* model = new QSqlRelationalTableModel(widget,QSqlDatabase::database("attribute"));
        model->setTable(tableName);
        model->select();
        widget->setModel(model);
        widget->resizeColumnsToContents();
        widget->resizeRowsToContents();
        widget->setEditTriggers(QTableView::NoEditTriggers);
        ui->tabWidget->addTab(widget,tableName);
        connect(widget,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
    }
}

void CDTAttributeDockWidget::clear()
{
    ui->tabWidget->setEnabled(false);
    _dbConnInfo = CDTDatabaseConnInfo();
    clearTables();
}

void CDTAttributeDockWidget::onActionEditDataSourceTriggered()
{
    DialogDBConnection dlg(_dbConnInfo);
    if (dlg.exec()==QDialog::Accepted)
    {
        qDebug()<<dlg.dbConnectInfo().dbName;
        if (dlg.dbConnectInfo() == _dbConnInfo)
            return;
        _segmentationLayer->setDatabaseURL(dlg.dbConnectInfo());
        setDatabaseURL(dlg.dbConnectInfo());
    }
}

void CDTAttributeDockWidget::onActionGenerateAttributesTriggered()
{
    clearTables();
    CDTImageLayer* layer = (CDTImageLayer*)(segmentationLayer()->parent());
    DialogGenerateAttributes dlg(segmentationLayer()->id(),layer->bandCount());
    dlg.exec();
    updateTable();
}

void CDTAttributeDockWidget::onDatabaseChanged(CDTDatabaseConnInfo connInfo)
{
    clearTables();

    if (connInfo.isNull())
        return;
    _dbConnInfo = connInfo;
    QSqlDatabase db = QSqlDatabase::addDatabase(connInfo.dbType,"attribute");
    db.setDatabaseName(connInfo.dbName);
    db.setHostName(connInfo.hostName);
    db.setPort(connInfo.port);
    if (!db.open(connInfo.username, connInfo.password)) {
        QSqlError err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase("attribute");
        QMessageBox::critical(this,tr("Error"),tr("Open database failed!\n information:")+err.text());
    }
    else
    {
        qDebug()<<"Open database "<<connInfo.dbType<<connInfo.dbName<<"suceed";
        updateTable();
    }
}

void CDTAttributeDockWidget::onItemClicked(QModelIndex index)
{    
    QTableView *view = (QTableView *)(ui->tabWidget->currentWidget());
    if (view==NULL)return;

    QString tableName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString featureName = view->model()->headerData(index.column(),Qt::Horizontal).toString();

    updateHistogram(featureName,tableName);
}

void CDTAttributeDockWidget::updateHistogram(const QString &featureName, const QString &tableName)
{
    qDebug()<<tableName<<featureName;

    QSqlDatabase db = QSqlDatabase::database("attribute");
    QSqlQuery query(db);
    query.exec(QString("select min(%1),max(%1) from %2").arg(featureName).arg(tableName));
    query.next();
    double minVal = query.value(0).toDouble();
    double maxVal = query.value(1).toDouble();
    int intervals = ui->qwtPlot->canvas()->width();
    intervals = intervals>120?120:intervals;
    double intervalStep = (maxVal - minVal)/intervals;
    qDebug()<<QString("min:%1 max:%2 width:%3").arg(minVal).arg(maxVal).arg(intervals);

    QVector<int> counts(intervals,0);
    query.exec(QString("select %1 from %2").arg(featureName).arg(tableName));
    while(query.next())
    {
        double val = query.value(0).toDouble();
        int id = (val-minVal)/intervalStep;
        if (id == intervals) --id;
        ++counts[id];
    }

    QVector<QPointF> datas;
    for (int i=0;i<intervals;++i)
        datas.push_back(QPointF(i*intervalStep+minVal,counts[i]));

    histogram->setSamples(new QwtPointSeriesData(datas));
    ui->qwtPlot->replot();
}

void CDTAttributeDockWidget::clearTables()
{
    while (ui->tabWidget->widget(0))
    {
        QWidget *widget = ui->tabWidget->widget(0);
        ui->tabWidget->removeTab(0);
        delete widget;
    }
}
