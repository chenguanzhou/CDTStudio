#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include "recentfilesupervisor.h"

namespace Ui {
class MainWindow;
}

class QModelIndex;
class QTreeView;
class CDTSampleDockWidget;
class CDTAttributeDockWidget;
class DialogConsole;
class CDTProjectWidget;
class QgsMapCanvas;
struct QUuid;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class RecentFileSupervisor;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();       

private:
    void initDockWidgets();

public:
    static MainWindow   *getMainWindow();
    static QTreeView    *getProjectTreeView();
    static CDTSampleDockWidget   *getSampleDockWidget();
    static CDTAttributeDockWidget *getAttributesDockWidget();
    static CDTProjectWidget *getCurrentProjectWidget();
    static QgsMapCanvas *getCurrentMapCanvas();

    static bool setActiveImage(QUuid uuid);
    static bool setActiveSegmentation(QUuid uuid);
signals:
    void loadSetting();
    void updateSetting();
public slots:
    void onCurrentTabChanged(int i);

private slots:
    void on_action_New_triggered();
    void on_treeViewProject_customContextMenuRequested(const QPoint &pos);
    void on_treeViewProject_clicked(const QModelIndex &index);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_All_triggered();
    void on_action_Save_As_triggered();
    void onRecentFileTriggered();

protected:
    void closeEvent(QCloseEvent *);


private:
    Ui::MainWindow *ui;
    CDTAttributeDockWidget *dockWidgetAttributes;
    CDTSampleDockWidget *dockWidgetSample;
    RecentFileSupervisor *supervisor;
    int recentFileCount;
    QToolButton* recentFileToolButton;
    QStringList recentFilePaths;
    DialogConsole* dialogConsole;

    static MainWindow* mainWindow;
    static bool isLocked;
};

#endif // MAINWINDOW_H
