#ifndef CDTVALIDATIONSAMPLEDOCKWIDGET_H
#define CDTVALIDATIONSAMPLEDOCKWIDGET_H

#include "cdtdockwidget.h"


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
    void onActionRename();
    void onActionAdd();
    void onActionRemove();
    void onActionExport();
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
