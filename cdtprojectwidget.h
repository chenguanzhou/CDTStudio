#ifndef CDTPROJECTWIDGET_H
#define CDTPROJECTWIDGET_H

#include <QWidget>
#include "cdtproject.h"
#include "cdtprojecttreemodel.h"
#include "mainwindow.h"


class CDTProjectWidget : public QWidget
{
    Q_OBJECT
public:
    friend class CDTProjectTabWidget;
    friend class MainWindow;
    explicit CDTProjectWidget(QWidget *parent = 0);    

signals:
    void projectChanged(CDTProject*);//?
public slots:
    void setProjectName(const QString& name);
    void setProjectPath(const QString& path);
//    void setimagelayer(const QVector<CDTImageLayer> &image);//using for test
private:
//    QVector<CDTImageLayer> image;//using for test
    CDTProject *project;
    CDTProjectTreeModel* treeModel;
};

#endif // CDTPROJECTWIDGET_H
