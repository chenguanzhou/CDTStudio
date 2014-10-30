#include "cdtplot2ddockwidget.h"
#include "cdthistogramplot.h"

CDTPlot2DDockWidget::CDTPlot2DDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    qwtPlot(new CDTHistogramPlot(this))
{        
    this->setWindowTitle(tr("Plot2D Panel"));
    qwtPlot->setObjectName(QString::fromUtf8("qwtPlot"));
    qwtPlot->setProperty("autoReplot", false);
    qwtPlot->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    qwtPlot->setMinimumHeight(200);
    this->setWidget(qwtPlot);
    qwtPlot->show();
}

CDTPlot2DDockWidget::~CDTPlot2DDockWidget()
{
    qwtPlot->clear();
}

void CDTPlot2DDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    Q_UNUSED(layer);
}

void CDTPlot2DDockWidget::onCurrentProjectClosed()
{
    qwtPlot->clear();
}

void CDTPlot2DDockWidget::setDataSource(QSqlDatabase db, QString tableName, QString fieldName)
{
    qwtPlot->setDatabase(db);
    qwtPlot->setTableName(tableName);
    qwtPlot->setFieldName(fieldName);
    qwtPlot->replot();
}
