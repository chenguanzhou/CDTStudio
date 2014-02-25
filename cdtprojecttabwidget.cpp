#include "cdtprojecttabwidget.h"
#include "dialognewproject.h"
#include <QFileDialog>
#include <QDataStream>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>

CDTProjectTabWidget::CDTProjectTabWidget(QWidget *parent) :
    QTabWidget(parent),openDefaultPath("."),saveDefaultPath(".")
{
    setTabsClosable(true);
    setTabShape(Triangular);
    currentWidget();
    setTabsClosable(true);
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}

bool CDTProjectTabWidget::createNewProject()
{
    DialogNewProject *dlg = new DialogNewProject(this);
    if (dlg->exec()==DialogNewProject::Accepted)
    {
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        projectWidget->setProjectPath(dlg->projectPath());
        projectWidget->setProjectName(dlg->projectName());
        projectWidget->setProjectFile(dlg->projectPath());
        addTab(projectWidget,dlg->projectName());

        return true;
    }
    delete dlg;
    return false;
}

bool CDTProjectTabWidget::openProject()
{
    QString filepath = QFileDialog::getOpenFileName(this,tr("Open an project file"),openDefaultPath,"*.cdtpro");
    openDefaultPath = filepath;
    if(!filepath.isEmpty())
    {
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        if (projectWidget->readProject(filepath) == false)
        {
            QMessageBox::critical(this,tr("Error File"),tr(" File Format Error!"));
            delete projectWidget;
            return false;
        }
        else
        {
            QFileInfo fileinfo(projectWidget->file);
            addTab(projectWidget,fileinfo.fileName());
            this->setCurrentWidget(projectWidget);
            return true;
        }
    }
    return false;

}

bool CDTProjectTabWidget::saveProject()
{    
    if(this->count()<=0)
    {
        return false;
    }
    else
    {
        ((CDTProjectWidget*)(this->currentWidget()))->writeProject();
        return true;
    }

}

bool CDTProjectTabWidget::saveAllProject()
{
    int count = this->count();
    if(count<=0)
    {
        return false;
    }
    else
    {
        for(int i=0;i<count;++i)
        {
            CDTProjectWidget *projectWidget =(CDTProjectWidget*)this->widget(i);
            projectWidget->writeProject();
        }
        return true;
    }
}

bool CDTProjectTabWidget::saveAsProject()
{
    if(this->count()<=0)
    {
        return false;
    }
    else
    {
        QString fileName = QFileDialog::getSaveFileName(this,tr("Save project file"),saveDefaultPath,"*.cdtpro");
        saveDefaultPath = fileName;
        ((CDTProjectWidget*)(this->currentWidget()))->saveAsProject(fileName);
        return true;
    }
}

void CDTProjectTabWidget::addProjectTab(const QString &path)
{
    //    CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
}

bool CDTProjectTabWidget::closeTab(const int &index)
{
    if(index < 0)
    {
        return false;
    }
    CDTProjectWidget* tabItem =(CDTProjectWidget*)this->widget(index);
    tabItem->closeProject(this,index);
    delete(tabItem);
    tabItem = nullptr;
    return true;
}

bool CDTProjectTabWidget::closeAll()
{

    int count = this->count();
    if(count<=0)
    {
        return false;
    }
    else
    {
        for(int i=0;i<count;++i)
        {
            this->closeTab(0);
        }
        return true;
    }
}
