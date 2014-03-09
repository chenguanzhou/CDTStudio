#include "cdtprojecttabwidget.h"
#include "dialognewproject.h"
#include <QFileDialog>
#include <QDataStream>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QSettings>
#include <QVariant>

CDTProjectTabWidget::CDTProjectTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}

bool CDTProjectTabWidget::createNewProject()
{
    DialogNewProject *dlg = new DialogNewProject(this);
    if (dlg->exec()==DialogNewProject::Accepted)
    {
        if(!CompareFilePath(QFileInfo(dlg->projectPath()).absoluteFilePath()))
        {
            QMessageBox::critical(this,tr("Error File"),tr("%1 have been opened!")
                                  .arg(QFileInfo(dlg->projectPath()).absoluteFilePath()));
            emit menuRecentChanged(QFileInfo(dlg->projectPath()).absoluteFilePath());
            return false;
        }

        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
        projectWidget->setProjectPath(dlg->projectPath());
        projectWidget->setProjectName(dlg->projectName());
        projectWidget->setProjectFile(dlg->projectPath());
        addTab(projectWidget,dlg->projectName());
        saveProject();
        emit menuRecentChanged(dlg->projectPath());
        delete dlg;
        return true;
    }
    delete dlg;
    return false;
}

bool CDTProjectTabWidget::openProject(QString &filepath)
{
    if(!filepath.isEmpty())
    {
        writeLastProjectDir(QFileInfo(filepath).absolutePath());
        CDTProjectWidget *projectWidget = new CDTProjectWidget(this);

        if (projectWidget->readProject(QFileInfo(filepath).absoluteFilePath()) == false)
        {

            QMessageBox::critical(this,tr("Error File"),tr(" File Format Error or invalid filepath!"));

            delete projectWidget;
            emit menuRecentChanged(filepath);
            return false;
        }
        else if(!CompareFilePath(QFileInfo(filepath).absoluteFilePath()))
        {
            QMessageBox::critical(this,tr("Error File"),tr("%1 have been opened!").arg(QFileInfo(filepath).absoluteFilePath()));
            delete projectWidget;
            emit menuRecentChanged(filepath);
            return false;
        }
        else
        {
            QFileInfo fileinfo(projectWidget->file);
            addTab(projectWidget,fileinfo.fileName());
            this->setCurrentWidget(projectWidget);
            emit menuRecentChanged(filepath);
            return true;
        }
    }
    return false;

}

bool CDTProjectTabWidget::openProject()
{
    QString dir = readLastProjectDir();
    QStringList filepaths = QFileDialog::getOpenFileNames(this,tr("Open an project file"),dir,"*.cdtpro");
    int openFailCount = 0;
    for(int i=0;i< filepaths.size();++i)
    {
        QString filepath = filepaths[i];
        if(!filepath.isEmpty())
        {
            writeLastProjectDir(QFileInfo(filepath).absolutePath());
            CDTProjectWidget *projectWidget = new CDTProjectWidget(this);
            if (projectWidget->readProject(QFileInfo(filepath).absoluteFilePath()) == false)
            {
                QMessageBox::critical(this,tr("Error File"),tr(" File Format Error!"));
                delete projectWidget;
                emit menuRecentChanged(filepath);
                ++openFailCount;
            }
            else if(!CompareFilePath(QFileInfo(filepath).absoluteFilePath()))
            {
                QMessageBox::critical(this,tr("Error File"),tr("%1 have been opened!").arg(QFileInfo(filepath).absoluteFilePath()));
                delete projectWidget;
                emit menuRecentChanged(filepath);
                ++openFailCount;
            }
            else
            {
                QFileInfo fileinfo(projectWidget->file);
                addTab(projectWidget,fileinfo.fileName());
                this->setCurrentWidget(projectWidget);
                emit menuRecentChanged(filepath);
            }
        }

    }
    if(openFailCount == 0)
        return true;
    else
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
        if(!CompareFilePath(QFileInfo(fileName).absoluteFilePath()))
        {
            QMessageBox::critical(this,tr("Error File"),tr("%1 have been opened!").arg(QFileInfo(fileName).absoluteFilePath()));
            emit menuRecentChanged(fileName);
            return false;
        }
        if(!fileName.isEmpty())
        {
            writeLastProjectDir(QFileInfo(fileName).absolutePath());
            emit menuRecentChanged(fileName);
            ((CDTProjectWidget*)(this->currentWidget()))->saveAsProject(fileName);
            return true;
        }
        return false;
    }
}

bool CDTProjectTabWidget::closeTab(const int &index)
{
    CDTProjectWidget* tabItem =(CDTProjectWidget*)this->widget(index);
    if(tabItem->closeProject(this,index))
    {
        delete (tabItem);
        tabItem = nullptr;
    }

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

bool CDTProjectTabWidget::CompareFilePath(QString &path)
{
    int count = this->count();
    for(int i=0; i< count;++i)
    {
        CDTProjectWidget* tab =(CDTProjectWidget*) this->widget(i);
        if(tab->filePath() == path)
            return false;
    }
    return true;
}












