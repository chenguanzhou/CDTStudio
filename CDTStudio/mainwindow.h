#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "recentfilesupervisor.h"
#include "log4qt/logger.h"

namespace Ui {
class MainWindow;
}
class QToolButton;
class QLineEdit;
class QModelIndex;
class QTreeView;
struct QUuid;

class QgsMapCanvas;
class QgsScaleComboBox;

class CDTDockWidget;
class CDTProjectWidget;
class CDTSampleDockWidget;
class CDTExtractionDockWidget;
class CDTLayerInfoWidget;
class CDTUndoWidget;
class CDTAttributeDockWidget;
class CDTProjectLayer;
class DialogConsole;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    friend class RecentFileSupervisor;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();       

private:
    void initIconSize();
    void initActions();
    void initMenuBar();
    void initToolBar();
    void initStatusBar();
    void initDockWidgets();
    void initConsole();    

    void registerDocks(Qt::DockWidgetArea area, CDTDockWidget* dock);



public:
    static MainWindow               *getMainWindow();
    static QTreeView                *getProjectTreeView();
    static CDTSampleDockWidget      *getSampleDockWidget();
    static CDTAttributeDockWidget   *getAttributesDockWidget();
    static CDTExtractionDockWidget  *getExtractionDockWidget();
    static CDTUndoWidget            *getUndoWidget();
    static CDTLayerInfoWidget       *getLayerInfoWidget();
    static CDTProjectWidget         *getCurrentProjectWidget();
    static QgsMapCanvas             *getCurrentMapCanvas();

    static QUuid getCurrentProjectID();
    static QSize getIconSize();

signals:
    void loadSetting();
    void updateSetting();
    void beforeProjectClosed(CDTProjectLayer*);
public slots:
    void onCurrentTabChanged(int i);
    void showMouseCoordinate(const QgsPoint & p);
    void showScale( double theScale );
    void userCenter();
    void userScale();

private slots:
    void onActionNew();
    void onActionOpen();
    void onActionSave();
    void onActionSaveAll();
    void onActionSaveAs();
    void onRecentFileTriggered();

    void onActionPBCD();
    void onActionOBCD();

    void on_treeViewObjects_customContextMenuRequested(const QPoint &pos);
    void on_treeViewObjects_clicked(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event);


private:
    Ui::MainWindow *ui;
    CDTAttributeDockWidget  *dockWidgetAttributes;
    CDTSampleDockWidget     *dockWidgetSample;
    CDTExtractionDockWidget *dockWidgetExtraction;
    CDTUndoWidget           *dockWidgetUndo;
    CDTLayerInfoWidget      *dockWidgetLayerInfo;

    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveAll;
    QAction *actionSaveAs;
    QAction *actionConsole;

    QAction *actionPBCD;//Pixel-based Change Detection
    QAction *actionOBCD;//Object-based Change Detection

    QMenu *menuFile;
    QMenu *menuRecent;

    QLineEdit *lineEditCoord;
    QgsScaleComboBox *scaleEdit;

    QSize iconSize;

    RecentFileSupervisor *supervisor;
    int recentFileCount;
    QToolButton* recentFileToolButton;
    QStringList recentFilePaths;

    QList<CDTDockWidget*> docks;
    static MainWindow* mainWindow;
    static bool isLocked;
};

#endif // MAINWINDOW_H
