#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    projectTabWidget(new CDTProjectTabWidget(this))
{
    ui->setupUi(this);
    setCentralWidget(projectTabWidget);
    connect(projectTabWidget,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
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
}

void MainWindow::on_action_New_triggered()
{
    projectTabWidget->createNewProject();
}
