#include "cdthistogramplot.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_picker_machine.h>
#include <qwt_symbol.h>
#include <QtSql>

class CDTHistogramPlotPrivate
{
public:
    QSqlDatabase db;
    QString tableName;
    QString fieldName;
};
CDTHistogramPlot::CDTHistogramPlot(QWidget *parent)
    :QwtPlot(parent),
      pData(new CDTHistogramPlotPrivate),
      histogram(new QwtPlotCurve)
{
    initHistogram();
}

CDTHistogramPlot::~CDTHistogramPlot()
{
    delete histogram;
}

QString CDTHistogramPlot::tableName() const
{
    return pData->tableName;
}

QString CDTHistogramPlot::fieldName() const
{
    return pData->fieldName;
}

void CDTHistogramPlot::setDatabase(QSqlDatabase db)
{
    pData->db = db;
}

void CDTHistogramPlot::setTableName(const QString &name)
{
    pData->tableName = name;
}

void CDTHistogramPlot::setFieldName(const QString &name)
{
    pData->fieldName = name;
}

void CDTHistogramPlot::replot()
{

    QwtPlot::replot();
}

void CDTHistogramPlot::initHistogram()
{
    histogram->attach(this);
    histogram->setStyle(QwtPlotCurve::Lines);
    histogram->setPen(QColor(255,0,0),2);
    histogram->setBrush(QBrush(QColor(255,0,0,127)));

    QwtPlotPicker *picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                               QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                               this );
    picker->setStateMachine( new QwtPickerTrackerMachine() );
    picker->setRubberBandPen( QColor( Qt::green ) );
    picker->setRubberBand( QwtPicker::VLineRubberBand );
    picker->setTrackerPen( QColor( Qt::blue ) );


    this->setFrameStyle(QwtPlotCanvas::NoFrame);
}

void CDTHistogramPlot::updateHistogram()
{

}
