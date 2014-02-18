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
//    QVector<CDTImageLayer> t_images;
//    t_images.push_back(CDTImageLayer("c:/","image1"));
//    t_images.push_back(CDTImageLayer("d:/","image2"));

    DialogNewProject *dlg = new DialogNewProject(this);
    if (dlg->exec()==DialogNewProject::Accepted)//Accepted?
    {
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        projectWidget->setProjectPath(dlg->projectPath());
        projectWidget->setProjectName(dlg->projectName());
//        projectWidget->setimagelayer(t_images);
        addTab(projectWidget,dlg->projectName());
//        projectWidgets.push_back(projectWidget);        
    }
}

void CDTProjectTabWidget::addProjectTab(const QString &path)
{
//    CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
}
