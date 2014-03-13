#ifndef RECENTFILESUPERVISOR_H
#define RECENTFILESUPERVISOR_H

#include <QObject>
#include "mainwindow.h"

namespace Ui {
class MainWindow;
}

class recentfilesupervisor : public QObject
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit recentfilesupervisor(MainWindow *w,QObject *parent = 0);

signals:
    void loadSettingFinished();
    void updataSettingFinished();
public slots:
    void loadSetting();
    void updataSetting();
    void updataMenuRecent(QString path);
private:
      MainWindow *window;

};

#endif // RECENTFILESUPERVISOR_H
