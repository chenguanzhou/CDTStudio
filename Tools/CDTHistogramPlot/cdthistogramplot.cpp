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
    if (histogram) delete histogram;
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
    if (updateHistogram())
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

bool CDTHistogramPlot::updateHistogram()
{
    bool ret;
    if (!pData->db.isValid())
    {
        qDebug()<<"DataBase not opened!";
        return false;
    }
    QSqlQuery query(pData->db);
    ret = query.exec(QString("select min(%1),max(%1) from %2")
               .arg(pData->fieldName).arg(pData->tableName));
    if (ret == false)
    {
        qDebug()<<"Query failed!";
        return false;
    }
    query.next();
    double minVal = query.value(0).toDouble();
    double maxVal = query.value(1).toDouble();
    int intervals = this->canvas()->width();
    intervals = intervals>120?120:intervals;
    double intervalStep = (maxVal - minVal)/intervals;
    qDebug()<<QString("min:%1 max:%2 width:%3").arg(minVal).arg(maxVal).arg(intervals);

    QVector<int> counts(intervals,0);
    ret = query.exec(QString("select %1 from %2")
               .arg(pData->fieldName).arg(pData->tableName));
    if (ret == false)
    {
        qDebug()<<"Query failed!";
        return false;
    }
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
    return true;
}
