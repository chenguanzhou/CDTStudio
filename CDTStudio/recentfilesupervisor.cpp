#include "recentfilesupervisor.h"
#include "cdtprojecttabwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QSettings>
#include<QFileInfo>

RecentFileSupervisor::RecentFileSupervisor(MainWindow *w) :
    QObject(w),window(w)
{    
}

void RecentFileSupervisor::loadSetting()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    window->recentFilePaths = setting.value("recentFiles",QStringList("")).toStringList();
    window->recentFileCount = setting.value("recentFlieCount","4").toInt();
    setting.endGroup();

    for(int i=0;i <window->recentFilePaths.size();++i)
    {
        QString path = window->recentFilePaths[i];
        QFileInfo info(path);
        if(!path.isEmpty()&&info.exists())
        {
            QAction* recentFile = new QAction(path,this);
            window->menuRecent->addAction(recentFile);
            window->recentFileToolButton->addAction(recentFile);
            connect(recentFile,SIGNAL(triggered()),window,SLOT(onRecentFileTriggered()));
        }
    }
}

void RecentFileSupervisor::updateSetting()
{
    window->recentFilePaths.clear();
    foreach (QAction* action, window->menuRecent->actions()) {
        QString path = action->text();
        QFileInfo info(path);
        if(!path.isEmpty()&&info.exists())
        {
            window->recentFilePaths.push_back(path);
        }
    }    

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    setting.setValue("recentFiles",window->recentFilePaths);
    setting.endGroup();
}

void RecentFileSupervisor::updateMenuRecent(QString path)
{
    QMenu *menuRecent = window->menuRecent;
    QList<QAction*> actions = menuRecent->actions();
    for (int i =0;i< actions.size();++i)
    {
        if(QFileInfo(path) == QFileInfo(actions[i] ->text()))
        {
            if(i==0)
                return;
            menuRecent->removeAction(actions[i]);
            break;
        }
    }
    QAction* recentFile = new QAction(path,this);
    menuRecent->insertAction(actions[0],recentFile);
    connect(recentFile,SIGNAL(triggered()),window,SLOT(onRecentFileTriggered()));
    if((menuRecent->actions()).size() >window->recentFileCount)
    {
        for(int i =window->recentFileCount;i < (menuRecent->actions()).size();++i)
        {
            menuRecent->removeAction((menuRecent->actions())[i]);
        }
    }
    actions = menuRecent->actions();
    while((window->recentFileToolButton->actions()).size()!=0)
        window->recentFileToolButton->removeAction(window->recentFileToolButton->actions()[0]);
    window->recentFileToolButton->addActions(actions);
}


