#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include "recentfilesupervisor.h"

namespace Ui {
class MainWindow;
}

class QModelIndex;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class RecentFileSupervisor;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void loadSetting();
    void updateSetting();
public slots:
    void onCurrentTabChanged(int i);

private slots:
    void on_action_New_triggered();
    void on_treeViewProject_customContextMenuRequested(const QPoint &pos);
    void on_treeViewProject_clicked(const QModelIndex &index);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_All_triggered();
    void on_action_Save_As_triggered();
    void onRecentFileTriggered();

private:
    Ui::MainWindow *ui;
    RecentFileSupervisor *supervisor;
    int recentFileCount;
    QToolButton* recentFileToolButton;
    QStringList recentFilePaths;
protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
