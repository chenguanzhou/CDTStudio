#ifndef CDTPROJECTWIDGET_H
#define CDTPROJECTWIDGET_H

#include <qgsmapcanvas.h>
#include <qgsmaptool.h>
#include "cdtprojectlayer.h"
#include "mainwindow.h"

class QStandardItemModel;
class CDTProjectWidget : public QWidget
{
    Q_OBJECT
public:
    friend class CDTProjectTabWidget;
    friend class MainWindow;

    explicit CDTProjectWidget(QWidget *parent = 0);
    ~CDTProjectWidget();

    bool readProject(const QString &filepath);
    bool writeProject();
    bool saveAsProject(const QString &path);
    int  maybeSave();
    QString filePath();
    QToolBar *menuBar();

signals:
    void projectChanged();
public slots:
//    void onContextMenu(QPoint, QModelIndex index);
    void onProjectChanged();
    bool saveProject(QString &path);

    void onZoomOutTool(bool toggle);
    void onZoomInTool(bool toggle);
    void onPanTool(bool toggle);
    void onFullExtent();
    void setLayerVisible(QgsMapLayer* layer,bool visible);
    void appendLayers(QList<QgsMapLayer*> layers);
    void removeLayer(QList<QgsMapLayer*> layer);
    void refreshMapCanvas(bool zoomToFullExtent=true);

    void onObjectItemChanged(QStandardItem* item);
//    void onChangesItemChanged(QStandardItem* item);

private slots:
    void untoggledToolBar();
private:
    CDTProjectLayer *project;
    QFile file;
    QStandardItemModel *treeModelObject;
//    QStandardItemModel *treeModelChanges;

    QgsMapCanvas* mapCanvas;
    QToolBar *initToolBar();
    QgsMapTool *panTool;
    QgsMapTool *zoomInTool;
    QgsMapTool *zoomOutTool;
    QAction *actionZoomOut;
    QAction *actionZoomIn;
    QAction *actionPan;
    QAction *actionFullExtent;

    QList<QgsMapLayer*>     activeLayers;
    QMap<QgsMapLayer*,bool> layersVisible;

    bool openProjectFile(QString filepath = QString());
    void createProject(QUuid id);
};

#endif // CDTPROJECTWIDGET_H
