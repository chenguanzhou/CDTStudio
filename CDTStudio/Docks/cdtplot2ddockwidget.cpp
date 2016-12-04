#include "cdtplot2ddockwidget.h"
#include "cdthistogramplot.h"
#include "stable.h"
#include "cdtattibutestatisticshelper.h"

CDTPlot2DDockWidget::CDTPlot2DDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    qwtPlot(new CDTHistogramPlot(this)),
    treeViewStatistices(new QTreeView(this)),
    model(new QStandardItemModel(this))
{        
    this->setEnabled(false);
    this->setWindowTitle(tr("Plot2D Panel"));

    qwtPlot->setObjectName(QString::fromUtf8("qwtPlot"));
    qwtPlot->setProperty("autoReplot", false);
    qwtPlot->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
//    qwtPlot->setMinimumHeight(200);

    treeViewStatistices->setModel(model);
    treeViewStatistices->setEditTriggers(QTreeView::NoEditTriggers);

    QWidget *widget = new QWidget(this);
    QHBoxLayout *hbox = new QHBoxLayout(widget);
    hbox->addWidget(qwtPlot);
    hbox->addWidget(treeViewStatistices);
    hbox->setStretchFactor(qwtPlot,2);
    hbox->setStretchFactor(treeViewStatistices,1);
    hbox->setSpacing(5);
    this->setWidget(widget);
    qDebug("Constructed");
}

CDTPlot2DDockWidget::~CDTPlot2DDockWidget()
{
    qwtPlot->clear();
    model->clear();
}

void CDTPlot2DDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    Q_UNUSED(layer);
    onDockClear();
}

void CDTPlot2DDockWidget::onDockClear()
{
    qwtPlot->clear();
    model->clear();
    this->setEnabled(false);
    this->setVisible(false);
}

void CDTPlot2DDockWidget::setDataSource(QSqlDatabase db, QString tableName, QString fieldName)
{
    if (!db.isValid())
    {
        qCritical("Database is invalid!");
        return;
    }
    if (!db.isOpen())
    {
        qCritical("Database is not opened!");
        return;
    }
    if (!db.tables().contains(tableName))
    {
        qCritical("Table %1 is not in database %2!",tableName,db.connectionName());
        return;
    }

    qDebug("Data source is set. Connection name: %1 Table name: %2 Field name: %3",
                   db.connectionName(),tableName,fieldName);
    qwtPlot->setDatabase(db);
    qwtPlot->setTableName(tableName);
    qwtPlot->setFieldName(fieldName);
    qwtPlot->replot();

    model->clear();
    model->setHorizontalHeaderLabels(QStringList()<<tr("Name")<<tr("Value"));
    model->appendRow(QList<QStandardItem*>()<<new QStandardItem("Computing.."));
    CDTAttibuteStatisticsHelper *helper = new CDTAttibuteStatisticsHelper(db,tableName,fieldName,this);
    connect(helper,SIGNAL(finished()),this,SLOT(onStatisticsFinished()));
    helper->start();
//    model->appendRow(QList<QStandardItem*>()<<new QStandardItem("fieldName")<<new QStandardItem(fieldName));

    this->setEnabled(true);
    this->setVisible(true);
    this->raise();
}

void CDTPlot2DDockWidget::onStatisticsFinished()
{
    CDTAttibuteStatisticsHelper *helper = qobject_cast<CDTAttibuteStatisticsHelper *>(sender());
    if (helper==NULL || helper->isValid())
        return;

    model->removeRows(0,model->rowCount());
    QMap<QString,QString> values = helper->values();
    foreach (QString key, values.keys()) {
        QString value = values.value(key);
        model->appendRow(QList<QStandardItem*>()<<new QStandardItem(key)<<new QStandardItem(value));
    }

    treeViewStatistices->resizeColumnToContents(0);
    treeViewStatistices->resizeColumnToContents(1);
}
