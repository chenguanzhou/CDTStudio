#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include <QMenu>
#include <QVector>
#include <QAction>
#include "cdtattributeswidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    setting.setValue("recentFlieCount","5");
    recentFileCount = setting.value("recentFlieCount","4").toInt();
    setting.endGroup();

    readSettings();
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(ui->tabWidgetProject,SIGNAL(menuRecentChanged(QString)),this,SLOT(updataMenuRecent(QString)));
    connect(ui->tabWidgetProject,SIGNAL(treeModelUpdated()),ui->treeViewProject,SLOT(expandAll()));
    connect(ui->tabWidgetProject,SIGNAL(treeModelUpdated()),this,SLOT(onTreeModelUpdated()));

    ui->horizontalLayoutAttributes->setMenuBar(ui->widgetAttributes->toolBar());
    ui->dockWidgetAttributes->setEnabled(false);
}



MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;    
}

void MainWindow::onCurrentTabChanged(int i)
{
    if(i==-1)
        return ;
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
    if (model==NULL)
        return;
    CDTProjectTreeItem *item =(CDTProjectTreeItem*)(model->itemFromIndex(index));
    if (item==NULL)
        return;
    int type = item->getType();

    ui->dockWidgetAttributes->setEnabled(false);
    if (type == CDTProjectTreeItem::SEGMENTION)
    {
        CDTSegmentationLayer* segmentationLayer = (CDTSegmentationLayer*)(item->getCorrespondingObject());        
        ui->widgetAttributes->setSegmentationLayer(segmentationLayer);
        ui->dockWidgetAttributes->setEnabled(true);
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


void MainWindow::readSettings()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QStringList paths = setting.value("recentFiles",QStringList("")).toStringList();
    for(int i=0;i <paths.size();++i)
    {
        QString path = paths[i];
        if(!path.isEmpty())
        {
            QAction* recentFile = new QAction(path,this);
            ui->menu_Recent->addAction(recentFile);
            connect(recentFile,SIGNAL(triggered()),this,SLOT(onRecentFile()));
        }
    }
    setting.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QStringList paths;
    foreach (QAction* action, ui->menu_Recent->actions()) {
        QString path = action->text();
        paths.push_back(path);
    }
    setting.setValue("recentFiles",paths);
    setting.endGroup();
}

void MainWindow::updataMenuRecent(QString path)
{
    QList<QAction*> actions = ui->menu_Recent->actions();
    for (int i =0;i< actions.size();++i)
    {
        if(path == actions[i] ->text())
        {
            ui->menu_Recent->removeAction(actions[i]);
            break;
        }
    }
    QAction* recentFile = new QAction(path,this);
    ui->menu_Recent->insertAction(actions[0],recentFile);
    connect(recentFile,SIGNAL(triggered()),this,SLOT(onRecentFile()));
    if((ui->menu_Recent->actions()).size() >recentFileCount)
    {
        for(int i =recentFileCount;i < (ui->menu_Recent->actions()).size();++i)
        {
            ui->menu_Recent->removeAction((ui->menu_Recent->actions())[i]);
        }
    }
}

void MainWindow::onTreeModelUpdated()
{
    qDebug()<<"lala";
}

void MainWindow::onRecentFile()
{
    QAction* action = (QAction*)sender();
    ui->tabWidgetProject->openProject(action->text());
}

void MainWindow::closeEvent(QCloseEvent *)
{
    ui->tabWidgetProject->closeAll();
}




