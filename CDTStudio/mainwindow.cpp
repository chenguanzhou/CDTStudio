#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include <QMenu>
#include <QVector>
#include <QAction>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    projectTabWidget(new CDTProjectTabWidget(this))
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    recentFileCount = setting.value("recentFlieCount","4").toInt();
    setting.endGroup();

    ui->setupUi(this);
    setCentralWidget(projectTabWidget);
    readRecentFiles();
    connect(projectTabWidget,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(projectTabWidget,SIGNAL(menuRecentChanged(QString)),this,SLOT(updataMenuRecent(QString)));
//    connect(qApp,SIGNAL(close()),projectTabWidget,SLOT(closeAll()));
    //connect(ui->treeViewProject,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(oncreatContextMenu(QPoint)));
//    connect(ui->treeViewProject,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCurrentTabChanged(int)));
}



MainWindow::~MainWindow()
{
    delete ui;
    writeRecentFile();
}

void MainWindow::onCurrentTabChanged(int i)
{
    if(i==-1)
        return ;
    CDTProjectWidget* projectWidget = (CDTProjectWidget*)(projectTabWidget->currentWidget());
    ui->treeViewProject->setModel(projectWidget->treeModel);
    ui->treeViewProject->expandAll();
    ui->treeViewProject->resizeColumnToContents(0);
}



void MainWindow::on_action_New_triggered()
{
    projectTabWidget->createNewProject();
}

void MainWindow::on_treeViewProject_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index =ui->treeViewProject->indexAt(pos);
    CDTProjectWidget* curwidget =(CDTProjectWidget*) projectTabWidget->currentWidget();
    if(curwidget == NULL)
        return;
    curwidget->onContextMenu(pos,index);
    ui->treeViewProject->expandAll();
}

void MainWindow::on_actionOpen_triggered()
{
    projectTabWidget->openProject();
}

void MainWindow::on_actionSave_triggered()
{
    projectTabWidget->saveProject();
}

void MainWindow::on_actionSave_All_triggered()
{
    projectTabWidget->saveAllProject();
}

void MainWindow::on_action_Save_As_triggered()
{
    projectTabWidget->saveAsProject();
}


void MainWindow::readRecentFiles()
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

void MainWindow::writeRecentFile()
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
    connect(recentFile,SIGNAL(triggered()),this,SLOT(on_action_RecentFile_triggered()));
    if(actions.size()+1 >recentFileCount)
    {
        for(int i =recentFileCount;i < actions.size();++i)
        {
            ui->menu_Recent->removeAction(actions[i]);
        }
    }
}

void MainWindow::on_action_RecentFile_triggered()
{
    QAction* action = (QAction*)sender();
    projectTabWidget->openProject(action->text());
}

void MainWindow::closeEvent(QCloseEvent *)
{
    projectTabWidget->closeAll();
}
