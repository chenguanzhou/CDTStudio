#ifndef DIALOGCONSOLE_H
#define DIALOGCONSOLE_H

#include <QDialog>
#include <QtSql>
namespace Ui {
class DialogConsole;
}

class QStringListModel;
class QModelIndex;

class DialogConsole : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConsole(QWidget *parent = 0);
    ~DialogConsole();

public slots:
    void on_pushButtonRefresh_clicked();
    void on_listView_clicked(const QModelIndex &index);

private slots:
    void on_pushButtonQuery_clicked();

private:
    Ui::DialogConsole *ui;
    QSqlDatabase db;
    QStringListModel* listModel;

    void updateTableList();
};



#endif // DIALOGCONSOLE_H
