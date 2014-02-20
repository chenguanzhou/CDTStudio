#include "cdtprojecttabwidget.h"
#include "dialognewproject.h"

CDTProjectTabWidget::CDTProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    setTabShape(Triangular);
}

void CDTProjectTabWidget::createNewProject()
{
    //for test

    DialogNewProject *dlg = new DialogNewProject(this);
    if (dlg->exec()==DialogNewProject::Accepted)//Accepted?
    {
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        projectWidget->setProjectPath(dlg->projectPath());
        projectWidget->setProjectName(dlg->projectName());
        addTab(projectWidget,dlg->projectName());   
    }
}

void CDTProjectTabWidget::addProjectTab(const QString &path)
{
//    CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
}
