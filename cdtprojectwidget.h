#ifndef CDTPROJECTWIDGET_H
#define CDTPROJECTWIDGET_H

#include <QWidget>
#include "cdtproject.h"
#include "cdtprojecttreemodel.h"
#include "mainwindow.h"
#include <QPoint>
#include <QModelIndex>


class CDTProjectWidget : public QWidget
{
    Q_OBJECT
public:
    friend class CDTProjectTabWidget;
    friend class MainWindow;
    explicit CDTProjectWidget(QWidget *parent = 0);
    void onContextMenu(QPoint pt,QModelIndex index);

signals:
    void projectChanged(CDTProject*);
public slots:
    void setProjectName(const QString& name);
    void setProjectPath(const QString& path);

private:
    CDTProject *project;
    CDTProjectTreeModel* treeModel;
};

#endif // CDTPROJECTWIDGET_H
