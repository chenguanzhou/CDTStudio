#ifndef CDTPLOT2DDOCKWIDGET_H
#define CDTPLOT2DDOCKWIDGET_H

#include "cdtdockwidget.h"
#include <QSqlDatabase>

class CDTHistogramPlot;
class CDTPlot2DDockWidget : public CDTDockWidget
{
    Q_OBJECT
public:
    explicit CDTPlot2DDockWidget(QWidget *parent = 0);
    ~CDTPlot2DDockWidget();

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();

    void setDataSource(QSqlDatabase db,QString tableName,QString fieldName);

private:
    CDTHistogramPlot *qwtPlot;
};

#endif // CDTPLOT2DDOCKWIDGET_H
