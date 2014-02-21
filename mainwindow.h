#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CDTProjectTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onCurrentTabChanged(int i);
private slots:
    void on_action_New_triggered();
    void on_treeViewProject_customContextMenuRequested(const QPoint &pos);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_All_triggered();

private:
    Ui::MainWindow *ui;
    CDTProjectTabWidget* projectTabWidget;
};

#endif // MAINWINDOW_H
