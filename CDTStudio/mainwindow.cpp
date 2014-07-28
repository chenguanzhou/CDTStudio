#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include "dialogconsole.h"
#include "stable.h"
#include "cdtsampledockwidget.h"
#include "cdtextractiondockwidget.h"
#include "cdtprojecttreeitem.h"
#include "cdtimagelayer.h"
#include "cdtextractionlayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassification.h"
#include "cdtundowidget.h"
#include "cdtlayerinfowidget.h"

MainWindow* MainWindow::mainWindow = NULL;
bool MainWindow::isLocked = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    recentFileToolButton(NULL),
    supervisor(NULL)
{        
    ui->setupUi(this);

    supervisor = new RecentFileSupervisor(this);
    recentFileToolButton = new QToolButton(this);
    mainWindow = this;

    initActions();
    initMenuBar();
    initToolBar();
    initDockWidgets();
    initConsole();

    connect(ui->tabWidgetProject,SIGNAL(beforeTabClosed(CDTProject*)),this,SIGNAL(beforeProjectClosed(CDTProject*)));

    recentFileToolButton->setText(tr("&Recent"));
    recentFileToolButton->setToolButtonStyle(ui->mainToolBar->toolButtonStyle());
    recentFileToolButton->setIcon(QIcon(":/Icon/RecentFiles.png"));
    recentFileToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->addWidget(recentFileToolButton);

    connect(ui->tabWidgetProject,SIGNAL(treeModelUpdated()),ui->treeViewProject,SLOT(expandAll()));
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(ui->tabWidgetProject,SIGNAL(menuRecentChanged(QString)),supervisor,SLOT(updateMenuRecent(QString)));
    connect(this,SIGNAL(loadSetting()),supervisor,SLOT(loadSetting()));
    connect(this,SIGNAL(updateSetting()),supervisor,SLOT(updateSetting()));

    QSettings setting("WHU","CDTStudio");
    this->restoreGeometry(setting.value("geometry").toByteArray());
    this->restoreState(setting.value("windowState").toByteArray());
    emit loadSetting();

    logger()->info("MainWindow initialized");
}


MainWindow::~MainWindow()
{    
    emit updateSetting();
    delete ui;
    logger()->info("MainWindow destruct");
}

void MainWindow::initActions()
{
    actionNew = new QAction(QIcon(":/Icon/New.png"),tr("&New"),this);
    actionNew->setShortcut(QKeySequence::New);
    actionNew->setStatusTip(tr("Create a new project"));
    connect(actionNew,SIGNAL(triggered()),SLOT(onActionNew()));

    actionOpen = new QAction(QIcon(":/Icon/Open.png"),tr("&Open"),this);
    actionOpen->setShortcut(QKeySequence::Open);
    actionOpen->setStatusTip(tr("Open one or more projects"));
    connect(actionOpen,SIGNAL(triggered()),SLOT(onActionOpen()));

    actionSave = new QAction(QIcon(":/Icon/Save.png"),tr("&Save"),this);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setStatusTip(tr("Save current project"));
    connect(actionSave,SIGNAL(triggered()),SLOT(onActionSave()));

    actionSaveAll = new QAction(QIcon(":/Icon/SaveAll.png"),tr("Save A&ll"),this);
    actionSaveAll->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_S));
    actionSaveAll->setStatusTip(tr("Save all existing projects"));
    connect(actionSaveAll,SIGNAL(triggered()),SLOT(onActionSaveAll()));

    actionSaveAs = new QAction(QIcon(":/Icon/SaveAs.png"),tr("Save &As"),this);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    actionSaveAs->setStatusTip(tr("Save current project as"));
    connect(actionSaveAs,SIGNAL(triggered()),SLOT(onActionSaveAs()));
}

void MainWindow::initMenuBar()
{
    menuFile = new QMenu(tr("&File"),this);
    menuFile->addActions(QList<QAction*>()
                         <<actionNew
                         <<actionOpen
                         <<actionSave
                         <<actionSaveAll
                         <<actionSaveAs);
    menuFile->addSeparator();
    menuRecent = new QMenu(tr("&Recent"),this);
    menuRecent->setIcon(QIcon(":/Icon/RecentFiles.png"));
    menuFile->addMenu(menuRecent);
    menuBar()->addMenu(menuFile);
}

void MainWindow::initToolBar()
{
    ui->mainToolBar->addActions(QList<QAction*>()
                                <<actionNew
                                <<actionOpen
                                <<actionSave
                                <<actionSaveAll
                                <<actionSaveAs);
}

void MainWindow::initDockWidgets()
{
    dockWidgetSample = new CDTSampleDockWidget(this);
    registerDocks(Qt::RightDockWidgetArea,dockWidgetSample);

    dockWidgetAttributes = new CDTAttributeDockWidget(this);
    registerDocks(Qt::BottomDockWidgetArea,dockWidgetAttributes);

    dockWidgetExtraction = new CDTExtractionDockWidget(this);
    registerDocks(Qt::RightDockWidgetArea,dockWidgetExtraction);

    dockWidgetUndo = new CDTUndoWidget(this,NULL);
    registerDocks(Qt::BottomDockWidgetArea,dockWidgetUndo);

    dockWidgetLayerInfo = new CDTLayerInfoWidget(this);
    dockWidgetLayerInfo->setObjectName("dockWidgetLayerInfo");
    registerDocks(Qt::LeftDockWidgetArea,dockWidgetLayerInfo);
}

void MainWindow::initConsole()
{
    DialogConsole* dialogConsole = new  DialogConsole(this);
    actionConsole = new QAction(tr("&Console"),this);
    actionConsole->setShortcut(QKeySequence(Qt::Key_F12));
    this->addAction(actionConsole);
    connect(actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(show()));
    connect(actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(updateDatabases()));
}

void MainWindow::registerDocks(Qt::DockWidgetArea area,CDTDockWidget *dock)
{
    connect(this,SIGNAL(beforeProjectClosed(CDTProject*)),dock,SLOT(onCurrentProjectClosed()));
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),dock,SLOT(onCurrentProjectClosed()));
    this->addDockWidget(area, dock);
    dock->raise();
    docks.push_back(dock);
}


MainWindow *MainWindow::getMainWindow()
{
    return mainWindow;
}

QTreeView *MainWindow::getProjectTreeView()
{
    return mainWindow->ui->treeViewProject;
}

CDTSampleDockWidget *MainWindow::getSampleDockWidget()
{
    return mainWindow->dockWidgetSample;
}

CDTAttributeDockWidget *MainWindow::getAttributesDockWidget()
{
    return mainWindow->dockWidgetAttributes;
}

CDTExtractionDockWidget *MainWindow::getExtractionDockWidget()
{
    return mainWindow->dockWidgetExtraction;
}

CDTUndoWidget *MainWindow::getUndoWidget()
{
    return mainWindow->dockWidgetUndo;
}

CDTLayerInfoWidget *MainWindow::getLayerInfoWidget()
{
    return mainWindow->dockWidgetLayerInfo;
}

CDTProjectWidget *MainWindow::getCurrentProjectWidget()
{
    return (CDTProjectWidget *)(mainWindow->ui->tabWidgetProject->currentWidget());
}

QgsMapCanvas *MainWindow::getCurrentMapCanvas()
{
    CDTProjectWidget *projectWidget = getCurrentProjectWidget();
    if (projectWidget == NULL) return NULL;
    return projectWidget->mapCanvas;
}

void MainWindow::onCurrentTabChanged(int i)
{
    if(i<0)
    {
        ui->treeViewProject->setModel(NULL);
        return ;
    }

    CDTProjectWidget* projectWidget = (CDTProjectWidget*)(ui->tabWidgetProject->currentWidget());
    ui->treeViewProject->setModel(projectWidget->treeModel);
    ui->treeViewProject->expandAll();
    ui->treeViewProject->resizeColumnToContents(0);
    logger()->info("Current tab is changed to %1",ui->tabWidgetProject->tabText(i));
}

void MainWindow::onActionNew()
{
    logger()->info("Create a new project");
    ui->tabWidgetProject->createNewProject();
}

void MainWindow::onActionOpen()
{
    ui->tabWidgetProject->openProject();
}

void MainWindow::onActionSave()
{
    ui->tabWidgetProject->saveProject();
}

void MainWindow::onActionSaveAll()
{
    ui->tabWidgetProject->saveAllProject();
}

void MainWindow::onActionSaveAs()
{
    ui->tabWidgetProject->saveAsProject();
}

void MainWindow::onRecentFileTriggered()
{
    QAction* action = (QAction*)sender();
    ui->tabWidgetProject->openProject(action->text());
}

void MainWindow::on_treeViewProject_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index =ui->treeViewProject->indexAt(pos);
    CDTProjectWidget* curwidget =(CDTProjectWidget*) ui->tabWidgetProject->currentWidget();
    if(curwidget == NULL)
        return;
    curwidget->onContextMenu(pos,index);
    ui->treeViewProject->expandAll();
}

void MainWindow::on_treeViewProject_clicked(const QModelIndex &index)
{
    QStandardItemModel* model = (QStandardItemModel*)(ui->treeViewProject->model());
    if (model==NULL)
        return;

    CDTProjectTreeItem *item =(CDTProjectTreeItem*)(model->itemFromIndex(index));
    if (item==NULL)
        return;

    if (item->correspondingObject() == NULL)
        return;

    foreach (CDTDockWidget *dock, docks) {
        dock->setCurrentLayer(item->correspondingObject());
    }

    int type = item->getType();
    if (type == CDTProjectTreeItem::EXTRACTION)
    {
        CDTExtractionLayer* extractionLayer = qobject_cast<CDTExtractionLayer*>(item->correspondingObject());
        if (extractionLayer != NULL)
        {
            //            dockWidgetExtraction->show();
            //            dockWidgetExtraction->setCurrentLayer(extractionLayer);
            extractionLayer->setOriginRenderer();
        }
    }
    else if (type == CDTProjectTreeItem::SEGMENTION)
    {
        CDTSegmentationLayer* segmentationLayer = qobject_cast<CDTSegmentationLayer*>(item->correspondingObject());
        if (segmentationLayer != NULL)
        {
            //            dockWidgetSample->setCurrentLayer(segmentationLayer);
            //            dockWidgetAttributes->setCurrentLayer(segmentationLayer);

            segmentationLayer->setOriginRenderer();
            if (segmentationLayer->canvasLayer()!=NULL)
            {
                CDTProjectWidget* widget = (CDTProjectWidget*)ui->tabWidgetProject->currentWidget();
                widget->mapCanvas->setCurrentLayer(segmentationLayer->canvasLayer());
            }
            getCurrentMapCanvas()->refresh();
        }
    }
    else if (type == CDTProjectTreeItem::CLASSIFICATION)
    {
        CDTClassification* classificationLayer = qobject_cast<CDTClassification*>(item->correspondingObject());
        if (classificationLayer != NULL)
        {
            CDTSegmentationLayer* segmentationLayer = (CDTSegmentationLayer*)(classificationLayer->parent());
            QgsFeatureRendererV2 *renderer = classificationLayer->renderer();
            segmentationLayer->setRenderer(renderer);
            getCurrentMapCanvas()->refresh();
        }
    }
    else if (type == CDTProjectTreeItem::IMAGE_ROOT)
    {
        //TODO  set current layer?
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->tabWidgetProject->closeAll();
    QSettings setting("WHU","CDTStudio");
    setting.setValue("geometry", saveGeometry());
    setting.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}
