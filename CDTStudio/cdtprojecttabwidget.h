#ifndef CDTPROJECTTABWIDGET_H
#define CDTPROJECTTABWIDGET_H

#include <QTabWidget>
#include "cdtprojectwidget.h"

class CDTProjectTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    friend class MainWindow;
//    friend QString openDefaultPath;
    explicit CDTProjectTabWidget(QWidget *parent = 0);

signals:
    void menuRecentChanged(QString path);
    void treeModelUpdated();
public slots:
    void createNewProject();
    void openProject(QString &filepath);
    void openProject();
    bool saveProject();
    bool saveAllProject();
    bool saveAsProject();
    bool closeTab(const int &index);
    void closeAll();
private:
    QString readLastProjectDir();
    void writeLastProjectDir(QString &path);
    void writeRecentFilePath(QString &path);
    void deleteRecentFilePath(QString &path);
    bool compareFilePath(QString &path);

};

#endif // CDTPROJECTTABWIDGET_H
