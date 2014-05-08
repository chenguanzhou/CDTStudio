#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include "cdttrainingsamplesform.h"
#include "cdtattributedockwidget.h"
#include <qgsmaplayer.h>
#include "dialogconsole.h"
#include "stable.h"

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

    dockWidgetAttributes = new CDTAttributeDockWidget(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, dockWidgetAttributes);
    dockWidgetAttributes->raise();
    dockWidgetAttributes->hide();

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
    connect(ui->actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(on_pushButtonRefresh_clicked()));

    connect(ui->tabWidgetProject,SIGNAL(treeModelUpdated()),ui->treeViewProject,SLOT(expandAll()));
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(ui->tabWidgetProject,SIGNAL(menuRecentChanged(QString)),supervisor,SLOT(updateMenuRecent(QString)));
    connect(this,SIGNAL(loadSetting()),supervisor,SLOT(loadSetting()));
    connect(this,SIGNAL(updateSetting()),supervisor,SLOT(updateSetting()));

//    ui->horizontalLayoutAttributes->setMenuBar(ui->widgetAttributes->menuBar());
//    ui->dockWidgetAttributes->setEnabled(false);

    emit loadSetting();
}


MainWindow::~MainWindow()
{
    emit updateSetting();
    delete ui;
}

MainWindow *MainWindow::getMainWindow()
{
    return mainWindow;
}

QTreeView *MainWindow::getProjectTreeView()
{
    return mainWindow->ui->treeViewProject;
}

CDTTrainingSamplesForm *MainWindow::getTrainingSampleForm()
{
    return mainWindow->ui->trainingSampleForm;
}

CDTAttributeDockWidget *MainWindow::getAttributesWidget()
{
    return mainWindow->dockWidgetAttributes;
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

    getTrainingSampleForm()->setImageID(uuid);
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
}

void MainWindow::on_action_New_triggered()
{
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
    if (type == CDTProjectTreeItem::SEGMENTION)
    {
        CDTSegmentationLayer* segmentationLayer = (CDTSegmentationLayer*)(item->getCorrespondingObject());
        if (segmentationLayer != NULL)
        {
            ui->trainingSampleForm->setSegmentationID(segmentationLayer->id());
            dockWidgetAttributes->setSegmentationLayer(segmentationLayer);
            dockWidgetAttributes->show();

            if (segmentationLayer->canvasLayer()!=NULL)
            {
                CDTProjectWidget* widget = (CDTProjectWidget*)ui->tabWidgetProject->currentWidget();
                widget->mapCanvas->setCurrentLayer(segmentationLayer->canvasLayer());
            }
        }
    }
    else if (type == CDTProjectTreeItem::IMAGE_ROOT)
    {
        //set current layer?
        CDTImageLayer* imageLayer = (CDTImageLayer*)(item->getCorrespondingObject());
        if (imageLayer != NULL)
        {
            ui->trainingSampleForm->setImageID(imageLayer->id());
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

void MainWindow::closeEvent(QCloseEvent *)
{
    ui->tabWidgetProject->closeAll();
}
