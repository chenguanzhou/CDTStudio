#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qgsscalecombobox.h"
#include "stable.h"

#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include "cdtattributedockwidget.h"
#include "cdtdockwidget.h"
#include "cdtsampledockwidget.h"
#include "cdtextractiondockwidget.h"
#include "cdtprojecttreeitem.h"
#include "cdtimagelayer.h"
#include "cdtextractionlayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"
#include "cdtundowidget.h"
#include "cdtlayerinfowidget.h"
#include "cdttaskdockwidget.h"

#include "dialogconsole.h"
#include "dialogpbcd.h"

#ifdef Q_OS_WIN
#include "Windows.h"
#endif

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

    initIconSize();
    initActions();
    initMenuBar();
    initToolBar();
    initStatusBar();
    initDockWidgets();
    initConsole();

    connect(ui->tabWidgetProject,SIGNAL(beforeTabClosed(CDTProjectLayer*)),this,SIGNAL(beforeProjectClosed(CDTProjectLayer*)));

    recentFileToolButton->setText(tr("&Recent"));
    recentFileToolButton->setToolButtonStyle(ui->mainToolBar->toolButtonStyle());
    recentFileToolButton->setIcon(QIcon(":/Icon/RecentFiles.png"));
    recentFileToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->addWidget(recentFileToolButton);

    connect(ui->tabWidgetProject,SIGNAL(treeModelUpdated()),ui->treeViewObjects,SLOT(expandAll()));
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(ui->tabWidgetProject,SIGNAL(menuRecentChanged(QString)),supervisor,SLOT(updateMenuRecent(QString)));
    connect(this,SIGNAL(loadSetting()),supervisor,SLOT(loadSetting()));
    connect(this,SIGNAL(updateSetting()),supervisor,SLOT(updateSetting()));

    connect(qApp,SIGNAL(taskInfoUpdated(QString ,int ,QString ,int ,int )),dockWIdgetTask,SLOT(updateTaskInfo(QString,int,QString,int,int)));
    connect(qApp,SIGNAL(taskCompleted(QString ,QByteArray )),dockWIdgetTask,SLOT(onTaskCompleted(QString ,QByteArray)));

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

void MainWindow::initIconSize()
{
    int dpiX = 96;
    int dpiY = 96;
#ifdef Q_OS_WIN
    dpiX = GetDeviceCaps(this->getDC(),LOGPIXELSX);
    dpiY = GetDeviceCaps(this->getDC(),LOGPIXELSY);

#endif
    iconSize = QSize(dpiX*16/96,dpiY*16/96);

    ui->mainToolBar->setIconSize(iconSize);
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

    actionPBCD = new QAction(QIcon(":/Icon/ChangePixel.png"),tr("&Pixel-based change detection"),this);
//    actionPBCD->setShortcut(QKeySequence::SaveAs);
    actionPBCD->setStatusTip(tr("Pixel-based change detection"));
    connect(actionPBCD,SIGNAL(triggered()),SLOT(onActionPBCD()));

    actionOBCD = new QAction(QIcon(":/Icon/ChangeObject.png"),tr("O&bject-based change detection"),this);
//    actionOBCD->setShortcut(QKeySequence::SaveAs);
    actionOBCD->setStatusTip(tr("Object-based change detection"));
    connect(actionOBCD,SIGNAL(triggered()),SLOT(onActionOBCD()));
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
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addActions(QList<QAction*>()
                                <<actionPBCD
                                <<actionOBCD);
    ui->mainToolBar->addSeparator();
}

void MainWindow::initStatusBar()
{
    //Coordinates
    QLabel *labelCoord = new QLabel( QString(), statusBar() );
    labelCoord->setObjectName( "mCoordsLabel" );
    labelCoord->setMinimumWidth( 10 );
    labelCoord->setMaximumHeight( 20 );
    labelCoord->setAlignment( Qt::AlignCenter );
    labelCoord->setFrameStyle( QFrame::NoFrame );
    labelCoord->setText( tr( "Coordinate:" ) );
    labelCoord->setToolTip( tr( "Current map coordinate" ) );
    statusBar()->addPermanentWidget( labelCoord, 0 );

    lineEditCoord = new QLineEdit( QString(), statusBar() );
    lineEditCoord->setObjectName( "lineEditCoord" );
    lineEditCoord->setMinimumWidth( 10 );
    lineEditCoord->setMaximumWidth( 300 );
    lineEditCoord->setMaximumHeight( 20 );
    lineEditCoord->setAlignment( Qt::AlignCenter );
    QRegExp coordValidator( "[+-]?\\d+\\.?\\d*\\s*,\\s*[+-]?\\d+\\.?\\d*" );
    new QRegExpValidator( coordValidator, lineEditCoord );
    lineEditCoord->setWhatsThis( tr( "Shows the map coordinates at the "
                                     "current cursor position. The display is continuously updated "
                                     "as the mouse is moved. It also allows editing to set the canvas "
                                     "center to a given position. The format is lat,lon or east,north" ) );
    lineEditCoord->setToolTip( tr( "Current map coordinate (lat,lon or east,north)" ) );
    statusBar()->addPermanentWidget( lineEditCoord, 0 );
    connect( lineEditCoord, SIGNAL( returnPressed() ), this, SLOT( userCenter() ) );

    // add a label to show current scale
    QLabel *scaleLabel = new QLabel( QString(), statusBar() );
    scaleLabel->setObjectName( "scaleLabel" );
    scaleLabel->setMinimumWidth( 10 );
    scaleLabel->setMaximumHeight( 20 );
    scaleLabel->setAlignment( Qt::AlignCenter );
    scaleLabel->setFrameStyle( QFrame::NoFrame );
    scaleLabel->setText( tr( "Scale:" ) );
    scaleLabel->setToolTip( tr( "Current map scale" ) );
    statusBar()->addPermanentWidget( scaleLabel, 0 );

    scaleEdit = new QgsScaleComboBox( statusBar() );
    scaleEdit->setObjectName( "scaleEdit" );
    scaleEdit->setMinimumWidth( 10 );
    scaleEdit->setMaximumWidth( 200 );
    scaleEdit->setMaximumHeight( 20 );
    scaleEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    scaleEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    scaleEdit->setWhatsThis( tr( "Displays the current map scale" ) );
    scaleEdit->setToolTip( tr( "Current map scale (formatted as x:y)" ) );

    statusBar()->addPermanentWidget( scaleEdit, 0 );
    connect( scaleEdit, SIGNAL( scaleChanged() ), this, SLOT( userScale() ) );
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

    dockWIdgetTask = new CDTTaskDockWidget(this);
    dockWIdgetTask->setObjectName("dockWIdgetTask");
    registerDocks(Qt::AllDockWidgetAreas,dockWIdgetTask);
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
    connect(this,SIGNAL(beforeProjectClosed(CDTProjectLayer*)),dock,SLOT(onCurrentProjectClosed()));
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
    return mainWindow->ui->treeViewObjects;
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

CDTTaskDockWidget *MainWindow::getTaskDockWIdget()
{
    return mainWindow->dockWIdgetTask;
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

QUuid MainWindow::getCurrentProjectID()
{
    CDTProjectWidget *prjWidget = getCurrentProjectWidget();
    if (prjWidget == 0)
        return QUuid();
    return prjWidget->project->id();
}

QSize MainWindow::getIconSize()
{
    return mainWindow->iconSize;
}

void MainWindow::onCurrentTabChanged(int i)
{
    if(i<0)
    {
        ui->treeViewObjects->setModel(NULL);
        ui->treeViewChanges->setModel(NULL);
        lineEditCoord->setText(QString::null);
        scaleEdit->lineEdit()->setText(QString::null);
        return ;
    }

    CDTProjectWidget* projectWidget = (CDTProjectWidget*)(ui->tabWidgetProject->currentWidget());
    ui->treeViewObjects->setModel(projectWidget->treeModelObject);
    ui->treeViewChanges->setModel(projectWidget->treeModelChanges);
    ui->treeViewObjects->expandAll();
    ui->treeViewObjects->resizeColumnToContents(0);
    if (getCurrentMapCanvas())
    {
        getCurrentMapCanvas()->updateScale();
        showScale(getCurrentMapCanvas()->scale());
    }

    logger()->info("Current tab is changed to %1",ui->tabWidgetProject->tabText(i));
}

void MainWindow::showMouseCoordinate(const QgsPoint &p)
{
    if (this->getCurrentMapCanvas()==NULL)
        return;

    lineEditCoord->setText( p.toString( ) );

    if ( lineEditCoord->width() > lineEditCoord->minimumWidth() )
    {
        lineEditCoord->setMinimumWidth( lineEditCoord->width() );
    }
}

void MainWindow::showScale(double theScale)
{
    scaleEdit->setScale( 1.0 / theScale );

    if ( scaleEdit->width() > scaleEdit->minimumWidth() )
    {
        scaleEdit->setMinimumWidth( scaleEdit->width() );
    }
}

void MainWindow::userCenter()
{
    QgsMapCanvas* mapCanvas = getCurrentMapCanvas();
    if (!mapCanvas)
        return;

    QStringList parts = lineEditCoord->text().split( ',' );
    if ( parts.size() != 2 )
        return;

    bool xOk;
    double x = parts.at( 0 ).toDouble( &xOk );
    if ( !xOk )
        return;

    bool yOk;
    double y = parts.at( 1 ).toDouble( &yOk );
    if ( !yOk )
        return;


    QgsRectangle r = mapCanvas->extent();

    mapCanvas->setExtent(
                QgsRectangle(
                    x - r.width() / 2.0,  y - r.height() / 2.0,
                    x + r.width() / 2.0, y + r.height() / 2.0
                    )
                );
    mapCanvas->refresh();
}

void MainWindow::userScale()
{
    QgsMapCanvas* mapCanvas = getCurrentMapCanvas();
    if (!mapCanvas)
        return;

    mapCanvas->zoomScale( 1.0 / scaleEdit->scale() );
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

void MainWindow::onActionPBCD()
{
    if (ui->tabWidgetProject->count()==0)
    {
        QMessageBox::critical(this,tr("Error"),tr("No project opend!"));
        return;
    }

    QUuid prjID = getCurrentProjectID();    

    DialogPBCD::openPBCDDialog(prjID);
}

void MainWindow::onActionOBCD()
{

}

void MainWindow::on_treeViewObjects_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index =ui->treeViewObjects->indexAt(pos);
    CDTProjectWidget* curwidget =(CDTProjectWidget*) ui->tabWidgetProject->currentWidget();
    if(curwidget == NULL)
        return;
//    curwidget->onContextMenu(pos,index);

    CDTProjectTreeItem *item =static_cast<CDTProjectTreeItem *>(static_cast<QStandardItemModel *>(ui->treeViewObjects->model())->itemFromIndex(index));
    if(item ==NULL)
        return;
    CDTBaseLayer* correspondingObject = item->correspondingObject();
    if (correspondingObject)
    {
        correspondingObject->onContextMenuRequest(this);
    }

    ui->treeViewObjects->expandAll();
}

void MainWindow::on_treeViewObjects_clicked(const QModelIndex &index)
{
    QStandardItemModel* model = (QStandardItemModel*)(ui->treeViewObjects->model());
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
        CDTClassificationLayer* classificationLayer = qobject_cast<CDTClassificationLayer*>(item->correspondingObject());
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
