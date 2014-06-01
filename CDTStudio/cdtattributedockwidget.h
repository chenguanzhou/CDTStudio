#ifndef CDTATTRIBUTEDOCKWIDGET_H
#define CDTATTRIBUTEDOCKWIDGET_H

#include <QDockWidget>
#include <QWidget>
#include <QtSql>
#include "dialogdbconnection.h"

class QToolBar;
class QMenuBar;
class CDTSegmentationLayer;

namespace Ui {
class CDTAttributeDockWidget;
}

class CDTAttributeDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CDTAttributeDockWidget(QWidget *parent = 0);
    ~CDTAttributeDockWidget();

    CDTSegmentationLayer *segmentationLayer()const;

signals:
    void databaseURLChanged(CDTDatabaseConnInfo);

public slots:
    void setDatabaseURL(CDTDatabaseConnInfo url);
    void setSegmentationLayer(CDTSegmentationLayer *layer);
    void updateTable();
    void clearTables();
    void clear();

private slots:
    void onActionEditDataSourceTriggered();
    void onActionGenerateAttributesTriggered();
    void onDatabaseChanged(CDTDatabaseConnInfo connInfo);
    void onItemClicked(QModelIndex index);

private:
    Ui::CDTAttributeDockWidget *ui;
    QMenuBar *_menuBar;
    CDTDatabaseConnInfo _dbConnInfo;
    CDTSegmentationLayer* _segmentationLayer;
};

#endif // CDTATTRIBUTEDOCKWIDGET_H
