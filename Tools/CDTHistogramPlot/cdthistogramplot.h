#ifndef CDTHISTOGRAMPLOT_H
#define CDTHISTOGRAMPLOT_H

#include <qwt_plot.h>
#include "cdthistogramplot_global.h"
#include <QSqlDatabase>

class QwtPlotCurve;
class CDTHistogramPlotPrivate;

class CDTHISTOGRAMPLOT_EXPORT CDTHistogramPlot:public QwtPlot
{
public:
    explicit CDTHistogramPlot( QWidget * parent = NULL );
    ~CDTHistogramPlot();

    QString tableName()const;
    QString fieldName()const;

public slots:
    void setDatabase(QSqlDatabase db);
    void setTableName(const QString &name);
    void setFieldName(const QString &name);
    void replot();

private:
    void initHistogram();
    void updateHistogram();
private:
    QwtPlotCurve *histogram;
    CDTHistogramPlotPrivate *pData;
};

#endif // CDTHISTOGRAMPLOT_H
