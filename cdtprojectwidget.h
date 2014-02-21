#ifndef CDTPROJECTWIDGET_H
#define CDTPROJECTWIDGET_H

#include <QWidget>
#include "cdtproject.h"
#include "cdtprojecttreemodel.h"
#include "mainwindow.h"
#include <QPoint>
#include <QModelIndex>
#include <QFile>


class CDTProjectWidget : public QWidget
{
    Q_OBJECT
public:
    friend class CDTProjectTabWidget;
    friend class MainWindow;
    explicit CDTProjectWidget(QWidget *parent = 0);
    void onContextMenu(QPoint pt,QModelIndex index);
    bool readProject(QString &filepath);
    void writeProject();
    ~CDTProjectWidget();

signals:
    void projectChanged(CDTProject*);
public slots:
    void setProjectName(const QString& name);
    void setProjectPath(const QString& path);
    void setProjectFile(const QString& filepath);

private:
    CDTProject *project;
    QFile file;
    CDTProjectTreeModel* treeModel;
};

#endif // CDTPROJECTWIDGET_H
