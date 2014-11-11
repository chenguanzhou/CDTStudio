#ifndef CDTHISTOGRAMPLOT_H
#define CDTHISTOGRAMPLOT_H

#include "cdthistogramplot_global.h"
#include <qwt_plot.h>
#include <QSqlDatabase>

class QwtPlotCurve;
class QwtPlotPicker;
class QwtPlotMagnifier;
class QwtPlotPanner;

class CDTHistogramPlotPrivate;

class CDTHISTOGRAMPLOT_EXPORT CDTHistogramPlot:public QwtPlot
{
    Q_OBJECT
public:
    CDTHistogramPlot(QWidget * parent = NULL );
    ~CDTHistogramPlot();

    QString tableName()const;
    QString fieldName()const;

public slots:
    void setDatabase(QSqlDatabase db);
    void setTableName(const QString &name);
    void setFieldName(const QString &name);
    void replot();
    void clear();
    void exportAsImage();

private:
    void initHistogram();
    void initTools();
    bool updateHistogram();

private:
    QwtPlotCurve *histogram;
    QwtPlotPicker *picker;
    QwtPlotMagnifier *magnifier;
    QwtPlotPanner *plotPanner;

    CDTHistogramPlotPrivate *pData;
};

#endif // CDTHISTOGRAMPLOT_H
