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

    bool isValid();
    QUuid currentCategoryID();

private:
    void setImageID(QUuid uuid);
    void setSegmentationID(QUuid uuid);

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();

    void updateTable();
    void updateComboBox();
    void updateListView();
    void clear();

private slots:
    void on_actionInsert_triggered();
    void on_actionRemove_triggered();
    void on_actionRemove_All_triggered();
    void on_actionRevert_triggered();
    void on_actionSubmit_triggered();
    void onPrimeInsert(int, QSqlRecord& record);
    void on_actionEdit_triggered(bool checked);

    void on_comboBox_currentIndexChanged(int index);
    void on_toolButtonEditSample_toggled(bool checked);
    void on_toolButtonSampleRename_clicked();
    void on_toolButtonNewSample_clicked();
    void on_toolButtonRemoveSelected_clicked();
    void on_listView_clicked(const QModelIndex &index);
    void on_groupBoxSamples_toggled(bool toggled);

private:
    Ui::CDTTrainingSampleDockWidget *ui;
    QSqlTableModel *categoryModel;
    QSqlQueryModel *sampleModel;
//    QAbstractItemDelegate* delegateColor;
    QUuid   imageLayerID;

    QgsMapTool *lastMapTool;
    CDTMapToolSelectTrainingSamples* currentMapTool;
};

#endif // CDTSAMPLEDOCKWIDGET_H
