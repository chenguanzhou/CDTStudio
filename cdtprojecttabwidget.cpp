#include "cdtprojecttabwidget.h"
#include "dialognewproject.h"
#include <QFileDialog>
#include <QDataStream>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QSettings>

CDTProjectTabWidget::CDTProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
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
        saveProject();
        emit menuRecentChanged();
        return true;
    }
    delete dlg;
    return false;
}

bool CDTProjectTabWidget::openProject(QString &filepath )
{
    if(filepath.isEmpty())
    {
        QString dir = readLastProjectDir();
        filepath = QFileDialog::getOpenFileName(this,tr("Open an project file"),dir,"*.cdtpro");
    }

    if(!filepath.isEmpty())
    {
        writeLastProjectDir(QFileInfo(filepath).absolutePath());
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        if (projectWidget->readProject(filepath) == false)
        {
            QMessageBox::critical(this,tr("Error File"),tr(" File Format Error or invalid filepath!"));
            delete projectWidget;
            deleteRecentFilePath(filepath);
            return false;
        }
        else
        {
            QFileInfo fileinfo(projectWidget->file);
            addTab(projectWidget,fileinfo.fileName());
            this->setCurrentWidget(projectWidget);
            writeRecentFilePath(filepath);
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
        QString dir = readLastProjectDir();
        QString fileName = QFileDialog::getSaveFileName(this,tr("Save project file"),dir,"*.cdtpro");
        if(!fileName.isEmpty())
        {
            writeLastProjectDir(QFileInfo(fileName).absolutePath());
            writeRecentFilePath(fileName);
            ((CDTProjectWidget*)(this->currentWidget()))->saveAsProject(fileName);
            return true;
        }
        return false;
    }
}

void CDTProjectTabWidget::addProjectTab(const QString &path)
{
    //    CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
}

bool CDTProjectTabWidget::closeTab(const int &index)
{
    CDTProjectWidget* tabItem =(CDTProjectWidget*)this->widget(index);
    tabItem->closeProject(this,index);

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

QString CDTProjectTabWidget::readLastProjectDir()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QString dir =setting.value("lastDir",".").toString();
    setting.endGroup();
    return dir;
}

void CDTProjectTabWidget::writeLastProjectDir(QString &path)
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    setting.setValue("lastDir",path);
    setting.endGroup();
}

void CDTProjectTabWidget::writeRecentFilePath(QString &path)
{
    QSettings setting("WHU","CDTStudio");
    setting.beginReadArray("recentFilePaths");
    QStringList paths;
    int size = setting.value("fileCount","4").toInt();
    for (int i = 0; i < size; ++i)
    {
        setting.setArrayIndex(i);
        QString p = setting.value("filePath").toString();
        paths.push_back(p);
    }
    paths.erase(paths.begin()+size,paths.end());
    setting.endArray();

    QStringList::const_iterator iter = qFind(paths,path);
    setting.beginWriteArray("recentFilePaths");
    if(iter == paths.end())
    {
        paths.insert(paths.begin(),path);
        for(int i=0;i<paths.size();++i)
        {
            setting.setArrayIndex(i);
            setting.setValue("filePath",paths[i]);
        }

    }
    else
    {
        int a = iter - paths.begin();
        paths.move(a,0);
        for(int i=0;i<paths.size();++i)
        {
            setting.setArrayIndex(i);
            setting.setValue("filePath",paths[i]);
        }

    }
    setting.endArray();
    emit menuRecentChanged();
}

void CDTProjectTabWidget::deleteRecentFilePath(QString &path)
{
    QSettings setting("WHU","CDTStudio");
    int size = setting.beginReadArray("recentFilePaths");
    for(int i=0;i < size;++i)
    {
        setting.setArrayIndex(i);
        if(setting.value("filePath").toString() == path)
        {
            setting.endArray();
            setting.beginWriteArray("recentFilePaths");
            setting.setArrayIndex(i);
            setting.remove("filePath");
            emit menuRecentChanged();
        }
    }
    setting.endArray();
}










