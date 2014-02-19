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
    void oncreatContextMenu(QPoint pt);
private slots:
    void on_action_New_triggered();

private:
    Ui::MainWindow *ui;
    CDTProjectTabWidget* projectTabWidget;
};

#endif // MAINWINDOW_H
