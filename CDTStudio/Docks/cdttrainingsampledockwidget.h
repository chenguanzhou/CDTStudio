#ifndef CDTSAMPLEDOCKWIDGET_H
#define CDTSAMPLEDOCKWIDGET_H

#include "cdtdockwidget.h"
#include <QColorDialog>

namespace Ui {
class CDTTrainingSampleDockWidget;
}

struct QUuid;
class QSqlRecord;
class QColor;
class QColorDialog;
class QSqlTableModel;
class QSqlQueryModel;
class QgsMapTool;

class CDTImageLayer;
class CDTMapToolSelectTrainingSamples;

class CDTTrainingSampleDockWidget : public CDTDockWidget
{
    Q_OBJECT

public:
    explicit CDTTrainingSampleDockWidget(QWidget *parent = 0);
    ~CDTTrainingSampleDockWidget();

    QUuid currentCategoryID();

private:
    void setSegmentationID(QUuid uuid);

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();

    void updateListView();
    void clear();

private slots:
    void on_toolButtonEditSample_toggled(bool checked);
    void on_toolButtonSampleRename_clicked();
    void on_toolButtonNewSample_clicked();
    void on_toolButtonRemoveSelected_clicked();
    void on_listView_clicked(const QModelIndex &index);
    void on_groupBoxSamples_toggled(bool toggled);

private:
    Ui::CDTTrainingSampleDockWidget *ui;
    QSqlQueryModel *sampleModel;
    QUuid   segmentationID;

    QgsMapTool *lastMapTool;
    CDTMapToolSelectTrainingSamples* currentMapTool;
};

#endif // CDTSAMPLEDOCKWIDGET_H
