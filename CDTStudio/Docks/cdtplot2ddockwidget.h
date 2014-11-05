#ifndef CDTPLOT2DDOCKWIDGET_H
#define CDTPLOT2DDOCKWIDGET_H

#include <QSqlDatabase>
#include "cdtdockwidget.h"
#include "log4qt/logger.h"

class CDTHistogramPlot;
class CDTPlot2DDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTPlot2DDockWidget(QWidget *parent = 0);
    ~CDTPlot2DDockWidget();

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

    void setDataSource(QSqlDatabase db,QString tableName,QString fieldName);

private:
    CDTHistogramPlot *qwtPlot;
};

#endif // CDTPLOT2DDOCKWIDGET_H
