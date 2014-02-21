#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    projectTabWidget(new CDTProjectTabWidget(this))
{
    ui->setupUi(this);
    setCentralWidget(projectTabWidget);
    connect(projectTabWidget,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(ui->treeViewProject,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(oncreatContextMenu(QPoint)));
    //connect(ui->treeViewProject,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCurrentTabChanged(int)));
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCurrentTabChanged(int i)
{
    //ui->treeViewProject->setModel(projectTabWidget->projectWidgets[i]->treeModel);
    CDTProjectWidget* projectWidget = (CDTProjectWidget*)(projectTabWidget->currentWidget());
    ui->treeViewProject->setModel(projectWidget->treeModel);
    ui->treeViewProject->expandAll();
    ui->treeViewProject->resizeColumnToContents(0);

}

void MainWindow::on_action_New_triggered()
{
    projectTabWidget->createNewProject();
}

void MainWindow::oncreatContextMenu(QPoint pt)
{
    QModelIndex index =ui->treeViewProject->indexAt(pt);
    CDTProjectWidget* curwidget =(CDTProjectWidget*) projectTabWidget->currentWidget();
    curwidget->onContextMenu(pt,index);
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
