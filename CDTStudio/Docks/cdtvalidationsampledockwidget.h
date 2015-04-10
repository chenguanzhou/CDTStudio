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
class QgsVectorLayer;

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
    void onSelectionChanged(QModelIndex,QModelIndex);
    void onGroupBoxToggled(bool toggled);
    void onActionAdd();
    void onActionRemove();
    void updateListView();

private:

    bool insertPointsIntoDB(const QString &pointsSetName, const QString &validationSampleID, const QString &validationSampleName);
    void createPointsLayer();
    void clearPointsLayer();
private:
    QGroupBox       *groupBox;
    QToolBar        *toolBar;
    QListView       *listView;
    QSqlQueryModel  *sampleModel;
    QUuid           imageID;

    QgsVectorLayer  *pointsLayer;
};

#endif // CDTVALIDATIONSAMPLEDOCKWIDGET_H
