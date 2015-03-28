#ifndef RECENTFILESUPERVISOR_H
#define RECENTFILESUPERVISOR_H

#include <QObject>

class MainWindow;
class RecentFileSupervisor : public QObject
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit RecentFileSupervisor(MainWindow *w);

signals:
    void loadSettingFinished();
    void updataSettingFinished();
public slots:
    void loadSetting();
    void updateSetting();
    void updateMenuRecent(QString path);
private:
      MainWindow *window;

};

#endif // RECENTFILESUPERVISOR_H
