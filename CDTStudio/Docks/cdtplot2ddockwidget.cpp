#include "cdtplot2ddockwidget.h"
#include "cdthistogramplot.h"
#include "stable.h"

CDTPlot2DDockWidget::CDTPlot2DDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    qwtPlot(new CDTHistogramPlot(this))
{        
    this->setEnabled(false);
    this->setWindowTitle(tr("Plot2D Panel"));

    qwtPlot->setObjectName(QString::fromUtf8("qwtPlot"));
    qwtPlot->setProperty("autoReplot", false);
    qwtPlot->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
//    qwtPlot->setMinimumHeight(200);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(widget);
    vbox->addWidget(qwtPlot);
    this->setWidget(widget);
    logger()->info("Constructed");
}

CDTPlot2DDockWidget::~CDTPlot2DDockWidget()
{
    qwtPlot->clear();
}

void CDTPlot2DDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    Q_UNUSED(layer);
    onDockClear();
}

void CDTPlot2DDockWidget::onDockClear()
{
    qwtPlot->clear();
    this->setEnabled(false);
    this->setVisible(false);
}

void CDTPlot2DDockWidget::setDataSource(QSqlDatabase db, QString tableName, QString fieldName)
{
    if (!db.isValid())
    {
        logger()->error("Database is invalid!");
        return;
    }
    if (!db.isOpen())
    {
        logger()->error("Database is not opened!");
        return;
    }
    if (!db.tables().contains(tableName))
    {
        logger()->error("Table %1 is not in database %2!",tableName,db.connectionName());
        return;
    }

    logger()->info("Data source is set. Connection name: %1 Table name: %2 Field name: %3",
                   db.connectionName(),tableName,fieldName);
    qwtPlot->setDatabase(db);
    qwtPlot->setTableName(tableName);
    qwtPlot->setFieldName(fieldName);
    qwtPlot->replot();
    this->setEnabled(true);
    this->setVisible(true);
    this->raise();
}
