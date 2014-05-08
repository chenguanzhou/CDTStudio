#ifndef FORMATTRIBUTES_H
#define FORMATTRIBUTES_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class CDTAttributesWidget;
}

class QToolBar;
class QMenuBar;
class CDTSegmentationLayer;
#include "dialogdbconnection.h"

class CDTAttributesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CDTAttributesWidget(QWidget *parent = 0);
    ~CDTAttributesWidget();

    QMenuBar *menuBar()const;
    CDTDatabaseConnInfo databaseURL() const;
    CDTSegmentationLayer *segmentationLayer()const;

signals:
    void databaseURLChanged(CDTDatabaseConnInfo);

public slots:
    void setDatabaseURL(CDTDatabaseConnInfo url);
    void setSegmentationLayer(CDTSegmentationLayer *layer);
    void updateTable();
    void clear();

private slots:
    void onActionEditDataSourceTriggered();
    void onActionGenerateAttributesTriggered();
    void onDatabaseChanged(CDTDatabaseConnInfo connInfo);    
    void clearTables();

private:
    Ui::CDTAttributesWidget *ui;
    QMenuBar *_menuBar;
    CDTDatabaseConnInfo _dbConnInfo;
    CDTSegmentationLayer* _segmentationLayer;
};

#endif // FORMATTRIBUTES_H
