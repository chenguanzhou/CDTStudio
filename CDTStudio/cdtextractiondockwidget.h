#ifndef CDTEXTRACTIONDOCKWIDGET_H
#define CDTEXTRACTIONDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class CDTExtractionDockWidget;
}
class QSqlQueryModel;
class QgsVectorLayer;
class QgsMapCanvas;
class QgsMapTool;

class CDTExtractionDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    enum EDITSTATE{
        LOCKED,EDITING
    };
    explicit CDTExtractionDockWidget(QWidget *parent = 0);
    ~CDTExtractionDockWidget();

    EDITSTATE editState()const;

public slots:
    void setExtractionLayer(QString id);
    void updateDescription(int currentIndex);

public slots:
    void setEditState(EDITSTATE state);
    void setGeometryModified(bool modified);

    void onActionStartEdit();
    void onActionRollBack();
    void onActionSave();
    void onActionStop();

private:
    void start();
    void rollback();
    void save();
    void stop();

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
    QgsMapTool      *lastMapTool;
    QgsMapTool      *currentMapTool;

    QAction         *actionStartEdit;
    QAction         *actionRollBack;
    QAction         *actionSave;
    QAction         *actionStop;
};

#endif // CDTEXTRACTIONDOCKWIDGET_H
