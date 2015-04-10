#ifndef CDTEXTRACTIONDOCKWIDGET_H
#define CDTEXTRACTIONDOCKWIDGET_H

#include "cdtdockwidget.h"
#include "log4qt/logger.h"

namespace Ui {
class CDTExtractionDockWidget;
}
class QSqlQueryModel;
class QgsVectorLayer;
class QgsMapCanvas;
class QgsMapTool;
class CDTUndoWidget;

class CDTExtractionDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum EDITSTATE{
        LOCKED,EDITING
    };
    explicit CDTExtractionDockWidget(QWidget *parent = 0);
    ~CDTExtractionDockWidget();

    EDITSTATE editState()const;

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();


    void updateDescription(int);
    void setEditState(EDITSTATE state);
    void setGeometryModified(bool modified);

    void onFeatureChanged();

    void onActionStartEdit();
    void onActionRollBack();
    void onActionSave();
    void onActionStop();

private:
    void start();
    void rollback();
    void save();
    void stop();

    void setExtractionLayer(QString id);

private:
    Ui::CDTExtractionDockWidget *ui;
    QSqlQueryModel*             modelExtractions;

    bool                    isGeometryModified;
    EDITSTATE               currentEditState;
    QMap<QString,QString>   description;

    QString         currentImagePath;
    QString         currentExtractionID;

    QgsVectorLayer  *vectorLayer;
    QgsMapCanvas    *mapCanvas;
    QCursor         lastCursor;
    QgsMapTool      *lastMapTool;
    QgsMapTool      *currentMapTool;

    QAction         *actionStartEdit;
    QAction         *actionRollBack;
    QAction         *actionSave;
    QAction         *actionStop;
};

#endif // CDTEXTRACTIONDOCKWIDGET_H
