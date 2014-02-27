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
    ui->setupUi(this);
    setCentralWidget(projectTabWidget);
    updataMenuRecent();
    connect(projectTabWidget,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(projectTabWidget,SIGNAL(menuRecentChanged()),this,SLOT(updataMenuRecent()));
//    connect(qApp,SIGNAL(close()),projectTabWidget,SLOT(closeAll()));
    //connect(ui->treeViewProject,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(oncreatContextMenu(QPoint)));
//    connect(ui->treeViewProject,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCurrentTabChanged(int)));
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCurrentTabChanged(int i)
{
    //ui->treeViewProject->setModel(projectTabWidget->projectWidgets[i]->treeModel);
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


void MainWindow::updataMenuRecent()
{
    QSettings setting("WHU","CDTStudio");
    int size = setting.beginReadArray("recentFilePaths");
    ui->menu_Recent->clear();
    for(int i=0;i < size;++i)
    {
        setting.setArrayIndex(i);
        QString path = setting.value("filePath").toString();
        if(path.isEmpty())
        {
            return ;
        }
        QAction* recentFile = new QAction(path,this);
        ui->menu_Recent->addAction(recentFile);
        ui->menu_Recent->addSeparator();
        connect(recentFile,SIGNAL(triggered()),this,SLOT(on_action_RecentFile_triggered()));
    }
    setting.endArray();
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
