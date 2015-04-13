#include "cdthistogramplot.h"
#include <QAction>
#include <QMessageBox>
#include <QtSql>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_layout.h>
#include <qwt_picker_machine.h>
#include <qwt_scale_widget.h>
#include <qwt_symbol.h>
#include <qwt_plot_renderer.h>


class CDTHistogramPlotPrivate
{
public:
    QSqlDatabase db;
    QString tableName;
    QString fieldName;
};
CDTHistogramPlot::CDTHistogramPlot(QWidget *parent)
    :QwtPlot(parent),
      histogram(new QwtPlotCurve),
      pData(new CDTHistogramPlotPrivate)
{
    initTools();
    initHistogram();

    histogram->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    QFrame *frame = (QFrame *)(this->canvas());
    frame->setFrameStyle(QFrame::StyledPanel);
    QFont font("Helvetica");
    this->setAxisFont(QwtPlot::xBottom,font);
    this->setAxisFont(QwtPlot::yLeft,font);

    QAction *action = new QAction(QIcon(":/Save.png"),tr("Save as image"),this);
    connect(action,SIGNAL(triggered()),SLOT(exportAsImage()));
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    this->addAction(action);
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
    {
        QwtText title;
        title.setText(QString("%1: %2").arg(pData->tableName).arg(pData->fieldName));
        QFont sansFont("Helvetica", 15);
        sansFont.setBold(false);
        title.setFont(sansFont);
//        title.setBorderRadius(4);
        this->setTitle(title);

        setAxisAutoScale(QwtPlot::xBottom);
        setAxisAutoScale(QwtPlot::yLeft);
        QwtPlot::replot();
    }
}

void CDTHistogramPlot::clear()
{
    QVector<QPointF> datas;
    histogram->setSamples(datas);
    this->setTitle(QString::null);
    QwtPlot::replot();
}

void CDTHistogramPlot::exportAsImage()
{
    QwtPlotRenderer renderer;
    if (renderer.exportTo(this,pData->tableName+"_"+pData->fieldName+".pdf")==false)
        QMessageBox::critical(this,tr("Error"),tr("Export failed!"));
    else
        QMessageBox::information(this,tr("Succeed"),tr("Export completed!"));
}

void CDTHistogramPlot::initHistogram()
{
    histogram->attach(this);
    histogram->setStyle(QwtPlotCurve::Lines);
    histogram->setPen(QPen(QColor(255,0,0),2));
    histogram->setBrush(QBrush(QColor(255,0,0,127)));


    this->setFrameStyle(QwtPlotCanvas::NoFrame);
}

void CDTHistogramPlot::initTools()
{
    plotPanner = new QwtPlotPanner( this->canvas() );
    magnifier = new QwtPlotMagnifier( this->canvas() );
    picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                this->canvas() );

    magnifier->setMouseButton( Qt::NoButton );

    picker->setStateMachine( new QwtPickerTrackerMachine() );
    picker->setRubberBandPen( QColor( Qt::green ) );
    picker->setRubberBand( QwtPicker::VLineRubberBand );
    picker->setTrackerPen( QColor( Qt::blue ) );
}

bool CDTHistogramPlot::updateHistogram()
{
    bool ret;
    if (!pData->db.isValid())
    {
        qDebug()<<"Database not opened!";
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

    histogram->setSamples(datas);
    return true;
}
