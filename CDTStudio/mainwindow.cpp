#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include <QMenu>
#include <QVector>
#include <QAction>


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

    ui->horizontalLayoutAttributes->setMenuBar(ui->dockWidgetAttributes->toolBar());
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
            connect(recentFile,SIGNAL(triggered()),this,SLOT(on_action_RecentFile_triggered()));
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
        if(QFileInfo(path) == QFileInfo(actions[i] ->text()))
        {
            if(i==0)
                return;
            ui->menu_Recent->removeAction(actions[i]);
            break;
        }
    }
    QAction* recentFile = new QAction(path,this);
    ui->menu_Recent->insertAction(actions[0],recentFile);
    connect(recentFile,SIGNAL(triggered()),this,SLOT(on_action_RecentFile_triggered()));
    if((ui->menu_Recent->actions()).size() >recentFileCount)
    {
        for(int i =recentFileCount;i < (ui->menu_Recent->actions()).size();++i)
        {
            ui->menu_Recent->removeAction((ui->menu_Recent->actions())[i]);
        }
    }
}

void MainWindow::on_action_RecentFile_triggered()
{
    QAction* action = (QAction*)sender();
    ui->tabWidgetProject->openProject(action->text());
}

void MainWindow::closeEvent(QCloseEvent *)
{
    ui->tabWidgetProject->closeAll();
}
