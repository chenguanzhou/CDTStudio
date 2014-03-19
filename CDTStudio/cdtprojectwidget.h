#ifndef CDTPROJECTWIDGET_H
#define CDTPROJECTWIDGET_H

#include <QWidget>
#include "cdtproject.h"
#include "cdtprojecttreemodel.h"
#include "mainwindow.h"
#include <QPoint>
#include <QModelIndex>
#include <QFile>
#include <qgsmapcanvas.h>
#include <qgsmaptool.h>

class CDTProjectWidget : public QWidget
{
    Q_OBJECT
public:
    friend class CDTProjectTabWidget;
    friend class MainWindow;
    explicit CDTProjectWidget(QWidget *parent = 0);

    bool readProject(QString &filepath);
    bool writeProject();
    bool saveAsProject(QString &path);
    int  maybeSave();
    QString filePath();
    bool closeProject(CDTProjectTabWidget *parent,const int &index);
    ~CDTProjectWidget();

signals:
    void projectChanged(CDTProject*);
public slots:
    void onContextMenu(QPoint pt,QModelIndex index);
    void setProjectName(const QString& name);
    void setProjectPath(const QString& path);
    void setProjectFile(const QString& filepath);
    void setIsChanged();
    bool saveProject(QString &path);
    bool saveFile(QString &filepath);

    void onZoomOutTool(bool toggle);
    void onZoomInTool(bool toggle);
    void onPanTool(bool toggle);
    void onFullExtent();
private slots:
    void untoggledToolBar();
private:
    bool isChanged;
    CDTProject *project;
    QFile file;
    CDTProjectTreeModel* treeModel;

//    QgsMapCanvas* mapCanvas;
    QToolBar *initToolBar();
    QgsMapTool *panTool;
    QgsMapTool *zoomInTool;
    QgsMapTool *zoomOutTool;
    QAction *actionZoomOut;
    QAction *actionZoomIn;
    QAction *actionPan;
    QAction *actionFullExtent;
};

#endif // CDTPROJECTWIDGET_H
