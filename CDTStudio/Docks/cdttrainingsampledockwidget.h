#ifndef CDTTRAININGSAMPLEDOCKWIDGET_H
#define CDTTRAININGSAMPLEDOCKWIDGET_H

#include <QColorDialog>
#include "cdtdockwidget.h"
#include "log4qt/logger.h"
//namespace Ui {
//class CDTSampleAbstractDockWidget;
//}

struct QUuid;
class QSqlRecord;
class QColor;
class QColorDialog;
class QGroupBox;
class QListView;
class QToolBar;
class QSqlTableModel;
class QSqlQueryModel;
class QgsMapTool;

class CDTImageLayer;
class CDTMapToolSelectTrainingSamples;

class CDTTrainingSampleDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTTrainingSampleDockWidget(QWidget *parent = 0);
    ~CDTTrainingSampleDockWidget();

private:
    void setSegmentationID(QUuid uuid);

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

    void updateListView();
    void clear();

private slots:
//    void on_toolButtonEditSample_toggled(bool checked);
    void onActionRename();
    void onActionAdd();
    void onActionRemove();
    void onListViewClicked(const QModelIndex &index);
    void onGroupBoxToggled(bool toggled);

private:
//    Ui::CDTSampleAbstractDockWidget *ui;
    QGroupBox *groupBoxSamples;
    QListView *listView;
    QSqlQueryModel *sampleModel;
    QToolBar *toolBar;
    QUuid   segmentationID;

    QgsMapTool *lastMapTool;
    CDTMapToolSelectTrainingSamples* currentMapTool;
};

#endif // CDTTRAININGSAMPLEDOCKWIDGET_H
