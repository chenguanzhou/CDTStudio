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
public slots:
    bool createNewProject();
    bool openProject(QString &filepath);
    bool openProject();
    bool saveProject();
    bool saveAllProject();
    bool saveAsProject();
    bool closeTab(const int &index);
    bool closeAll();
private:
    QString readLastProjectDir();
    void writeLastProjectDir(QString &path);
    void writeRecentFilePath(QString &path);
    void deleteRecentFilePath(QString &path);
};

#endif // CDTPROJECTTABWIDGET_H
