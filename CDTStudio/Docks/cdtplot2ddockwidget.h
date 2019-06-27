#ifndef CDTPLOT2DDOCKWIDGET_H
#define CDTPLOT2DDOCKWIDGET_H

#include <QSqlDatabase>
#include "cdtdockwidget.h"


class CDTHistogramPlot;
class QTreeView;
class QStandardItemModel;
class CDTPlot2DDockWidget : public CDTDockWidget
{
    Q_OBJECT
    
public:
    explicit CDTPlot2DDockWidget(QWidget *parent = 0);
    ~CDTPlot2DDockWidget();

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

    void setDataSource(QSqlDatabase db,QString tableName,QString fieldName);

    void onStatisticsFinished();
private:
    CDTHistogramPlot *qwtPlot;
    QTreeView *treeViewStatistices;
    QStandardItemModel *model;
};

#endif // CDTPLOT2DDOCKWIDGET_H
