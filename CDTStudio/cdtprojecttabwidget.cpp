#include "cdtprojecttabwidget.h"
#include "stable.h"
#include "dialognewproject.h"


CDTProjectTabWidget::CDTProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));    
}

void CDTProjectTabWidget::createNewProject()
{
    DialogNewProject *dlg = new DialogNewProject(this);
    if (dlg->exec()==DialogNewProject::Accepted)
    {
        if (dlg->projectPath().isEmpty() || dlg->projectName().isEmpty())
        {
            QMessageBox::information(this,tr("Error"),tr("Project path or name is empty!"));
            return ;
        }
        if(!compareFilePath(QFileInfo(dlg->projectPath()).absoluteFilePath()))
            return;

        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        projectWidget->createProject(QUuid::createUuid());
        projectWidget->project->initLayer(dlg->projectName());

        if (projectWidget->openProjectFile(dlg->projectPath())==false)return;        
        addTab(projectWidget,dlg->projectName());
        saveProject();
        emit menuRecentChanged(dlg->projectPath());
    }
}

void CDTProjectTabWidget::openProject(const QString &filepath)
{
    if(!filepath.isEmpty())
    {
        writeLastProjectDir(QFileInfo(filepath).absolutePath());
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        if (projectWidget->readProject(QFileInfo(filepath).absoluteFilePath()) == false)
        {
            QMessageBox::critical(this,tr("Error File"),tr(" File Format Error or invalid filepath!"));
            delete projectWidget;
            return;
        }

        if(!compareFilePath(QFileInfo(filepath).absoluteFilePath()))
            return;

        addTab(projectWidget,projectWidget->project->name());
        this->setCurrentWidget(projectWidget);
        emit menuRecentChanged(filepath);
    }
}

void CDTProjectTabWidget::openProject()
{
    QString dir = readLastProjectDir();
    QStringList filepaths = QFileDialog::getOpenFileNames(this,tr("Open an project file"),dir,"*.cdtpro");
    for(int i=0;i< filepaths.size();++i)
    {
        QString filepath = filepaths[i];
        openProject(filepath);
    }
}

bool CDTProjectTabWidget::saveProject()
{    
    if(this->count()<=0)
        return false;

    ((CDTProjectWidget*)(this->currentWidget()))->writeProject();
    return true;
}

bool CDTProjectTabWidget::saveAllProject()
{
    if(this->count()<=0)
        return false;

    for(int i=0;i<this->count();++i)
    {
        CDTProjectWidget *projectWidget =(CDTProjectWidget*)this->widget(i);
        projectWidget->writeProject();
    }
    return true;

}

bool CDTProjectTabWidget::saveAsProject()
{
    if(this->count()<=0)
        return false;

    QString dir = readLastProjectDir();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save project file"),dir,"*.cdtpro");
    if(compareFilePath(QFileInfo(fileName).absoluteFilePath())==false)
        return false;

    if(!fileName.isEmpty())
    {
        writeLastProjectDir(QFileInfo(fileName).absolutePath());
        emit menuRecentChanged(fileName);        
        ((CDTProjectWidget*)(this->currentWidget()))->saveAsProject(fileName);
    }
    return true;
}

int CDTProjectTabWidget::closeTab(const int &index)
{
    if(index<0) return false;

    CDTProjectWidget* tabItem =(CDTProjectWidget*)this->widget(index);

    int ret = tabItem->maybeSave();

    if (ret != QMessageBox::Cancel)
    {
        this->removeTab(index);
        delete tabItem;
    }

    return ret;
}

bool CDTProjectTabWidget::closeAll()
{
    if(this->count()<=0)
        return true;

    int tabIndex = 0;
    bool isAppClose = true;
    for(int i=0;i<this->count();++i)
    {
        if (this->closeTab(tabIndex)==QMessageBox::Cancel)
        {
            ++tabIndex;
            isAppClose = false;
        }
    }
    return isAppClose;
}

QString CDTProjectTabWidget::readLastProjectDir()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QString dir =setting.value("lastDir",".").toString();
    setting.endGroup();
    return dir;
}

void CDTProjectTabWidget::writeLastProjectDir(const QString &path)
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    setting.setValue("lastDir",path);
    setting.endGroup();
}


bool CDTProjectTabWidget::compareFilePath(const QString &path)
{
    int count = this->count();
    for(int i=0; i< count;++i)
    {
        CDTProjectWidget* tab =(CDTProjectWidget*) this->widget(i);
        if(tab->filePath() == path)
        {
            QMessageBox::critical(this,tr("Error File"),tr("%1 have been opened!")
                                  .arg(QFileInfo(path).absoluteFilePath()));
            return false;
        }
    }
    return true;
}












