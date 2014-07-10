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

MainWindow* MainWindow::mainWindow = NULL;
bool MainWindow::isLocked = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    recentFileToolButton(NULL),
    supervisor(NULL),
    dialogConsole(NULL)
{        
    ui->setupUi(this);
    initDockWidgets();

    supervisor = new RecentFileSupervisor(this);
    dialogConsole = new  DialogConsole(this);
    recentFileToolButton = new QToolButton(this);
    mainWindow = this;

    recentFileToolButton->setText(tr("&Recent"));
    recentFileToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    recentFileToolButton->setIcon(QIcon(":/Icon/recentfile.png"));
    recentFileToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->addWidget(recentFileToolButton);

    this->addAction(ui->actionConsole);
    connect(ui->actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(show()));
    connect(ui->actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(updateDatabases()));

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

void MainWindow::initDockWidgets()
{
    dockWidgetAttributes = new CDTAttributeDockWidget(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, dockWidgetAttributes);
    dockWidgetAttributes->raise();
    dockWidgetAttributes->hide();

    dockWidgetSample = new CDTSampleDockWidget(this);
    this->addDockWidget(Qt::RightDockWidgetArea, (QDockWidget*)dockWidgetSample);
    dockWidgetSample->raise();
    dockWidgetSample->hide();

    dockWidgetExtraction = new CDTExtractionDockWidget(this);
    dockWidgetExtraction->setAllowedAreas(Qt::RightDockWidgetArea|Qt::LeftDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, (QDockWidget*)dockWidgetExtraction);
    dockWidgetExtraction->setEnabled(false);
    dockWidgetExtraction->raise();
    dockWidgetExtraction->hide();
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

bool MainWindow::setActiveImage(QUuid uuid)
{
    if (isLocked)
    {
        qWarning()<<tr("It's locked!");
        return false;
    }

    getSampleDockWidget()->setImageID(uuid);
//    ui->dockWidgetAttributes->show();
//    ui->dockWidgetTrainingSample->show();
    return true;
}

bool MainWindow::setActiveSegmentation(QUuid uuid)
{
//    ui->dockWidgetAttributes->show();
//    ui->dockWidgetTrainingSample->show();
    return true;
}

void MainWindow::onCurrentTabChanged(int i)
{
    if(i==-1)   return ;    

    CDTProjectWidget* projectWidget = (CDTProjectWidget*)(ui->tabWidgetProject->currentWidget());
    ui->treeViewProject->setModel(projectWidget->treeModel);
    ui->treeViewProject->expandAll();
    ui->treeViewProject->resizeColumnToContents(0);
    logger()->info("Current tab is changed to %1",ui->tabWidgetProject->tabText(i));
}

void MainWindow::on_action_New_triggered()
{
    logger()->info("Create a new project");
    ui->tabWidgetProject->createNewProject();
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
    if (model==NULL)     return;
    CDTProjectTreeItem *item =(CDTProjectTreeItem*)(model->itemFromIndex(index));
    if (item==NULL)    return;

    int type = item->getType();
    if (type == CDTProjectTreeItem::EXTRACTION)
    {
        CDTExtractionLayer* extractionLayer = (CDTExtractionLayer*)(item->correspondingObject());
        if (extractionLayer != NULL)
        {
            dockWidgetExtraction->setEnabled(true);
            dockWidgetExtraction->show();
            dockWidgetExtraction->setExtractionLayer(extractionLayer->id());

            extractionLayer->setOriginRenderer();
        }
    }
    else if (type == CDTProjectTreeItem::SEGMENTION)
    {
        CDTSegmentationLayer* segmentationLayer = (CDTSegmentationLayer*)(item->correspondingObject());
        if (segmentationLayer != NULL)
        {
            dockWidgetSample->setSegmentationID(segmentationLayer->id());
            dockWidgetSample->show();            
            dockWidgetAttributes->setSegmentationLayer(segmentationLayer);
            dockWidgetAttributes->show();

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
        CDTClassification* classificationLayer = (CDTClassification*)(item->correspondingObject());
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
        //set current layer?
        CDTImageLayer* imageLayer = (CDTImageLayer*)(item->correspondingObject());
        if (imageLayer != NULL)
        {
            dockWidgetSample->show();
            dockWidgetSample->setImageID(imageLayer->id());
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    ui->tabWidgetProject->openProject();
}

void MainWindow::on_actionSave_triggered()
{
    ui->tabWidgetProject->saveProject();
}

void MainWindow::on_actionSave_All_triggered()
{
    ui->tabWidgetProject->saveAllProject();
}

void MainWindow::on_action_Save_As_triggered()
{
    ui->tabWidgetProject->saveAsProject();
}

void MainWindow::onRecentFileTriggered()
{
    QAction* action = (QAction*)sender();
    ui->tabWidgetProject->openProject(action->text());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->tabWidgetProject->closeAll();
    QSettings setting("WHU","CDTStudio");
    setting.setValue("geometry", saveGeometry());
    setting.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}
