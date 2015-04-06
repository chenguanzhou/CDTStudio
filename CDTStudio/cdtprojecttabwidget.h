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
//    void beforeTabClosed(CDTProjectLayer*);
public slots:
    void createNewProject();
    void openProject(const QString &filepath);
    void openProject();
    bool saveProject();
    bool saveAllProject();
    bool saveAsProject();
    int closeTab(const int &index);
    bool closeAll();
private:
    QString readLastProjectDir();
    void writeLastProjectDir(const QString &path);
    void writeRecentFilePath(const QString &path);
    bool compareFilePath(const QString &path);

};

#endif // CDTPROJECTTABWIDGET_H
