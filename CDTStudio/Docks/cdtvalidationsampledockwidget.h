#ifndef CDTVALIDATIONSAMPLEDOCKWIDGET_H
#define CDTVALIDATIONSAMPLEDOCKWIDGET_H

#include "cdtdockwidget.h"
#include "log4qt/logger.h"

class QGroupBox;
class QToolBar;
class QListView;
class QgsMapTool;
class QSqlQueryModel;
class QgsRectangle;

class CDTValidationSampleDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTValidationSampleDockWidget(QWidget *parent = 0);
    ~CDTValidationSampleDockWidget();

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

private slots:
    void onGroupBoxToggled(bool toggled);
    void onActionAdd();

private:
    QVector<QPointF> generatePoints(int pointsCount, const QgsRectangle &extent);
    bool insertPointsIntoDB(QVector<QPointF> points,const QString &pointsSetName);
private:
    QGroupBox       *groupBox;
    QToolBar        *toolBar;
    QListView       *listView;
    QSqlQueryModel  *sampleModel;
    QUuid           imageID;

    QgsMapTool      *lastMapTool;
    QgsMapTool      *currentMapTool;
};

#endif // CDTVALIDATIONSAMPLEDOCKWIDGET_H
