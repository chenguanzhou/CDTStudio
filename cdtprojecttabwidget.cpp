#include "cdtprojecttabwidget.h"
#include "dialognewproject.h"
#include <QFileDialog>
#include <QDataStream>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>

CDTProjectTabWidget::CDTProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setTabShape(Triangular);
    currentWidget();
    setTabsClosable(true);
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}

void CDTProjectTabWidget::createNewProject()
{
    DialogNewProject *dlg = new DialogNewProject(this);
    if (dlg->exec()==DialogNewProject::Accepted)
    {
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        projectWidget->setProjectPath(dlg->projectPath());
        projectWidget->setProjectName(dlg->projectName());
        projectWidget->setProjectFile(dlg->projectPath());
        addTab(projectWidget,dlg->projectName());
    }
}

void CDTProjectTabWidget::openProject()
{
    QString filepath = QFileDialog::getOpenFileName(this,tr("Open an project file"),QString(),"*.cdtpro");
    if(!filepath.isEmpty())
    {
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        if (projectWidget->readProject(filepath) == false)
        {
            QMessageBox::critical(this,tr("Error File"),tr(" File Format Error!"));
            delete projectWidget;
        }
        else
        {
            QFileInfo fileinfo(projectWidget->file);
            addTab(projectWidget,fileinfo.fileName());
            this->setCurrentWidget(projectWidget);
        }
    }

}

void CDTProjectTabWidget::saveProject()
{    
    ((CDTProjectWidget*)(this->currentWidget()))->writeProject();
}

void CDTProjectTabWidget::saveAllProject()
{
    int count = this->count();
    for(int i=0;i<count;++i)
    {
        CDTProjectWidget *projectWidget =(CDTProjectWidget*)this->widget(i);
        projectWidget->writeProject();
    }
}

void CDTProjectTabWidget::addProjectTab(const QString &path)
{
    //    CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
}

void CDTProjectTabWidget::closeTab(const int &index)
{
    if(index < 0)
    {
        return ;
    }
    CDTProjectWidget* tabItem =(CDTProjectWidget*)this->widget(index);
    this->removeTab(index);
    delete(tabItem);
    tabItem = nullptr;
}
