#ifndef DIALOGCONSOLE_H
#define DIALOGCONSOLE_H

#include <QDialog>

namespace Ui {
class DialogConsole;
}
class QSqlDatabase;
class QStringListModel;
class QSqlTableModel;

class DialogConsole : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConsole(QWidget *parent = 0);
    ~DialogConsole();

private slots:
    void on_pushButtonRefresh_clicked();
    void on_listView_clicked(const QModelIndex &index);

private:

    Ui::DialogConsole *ui;
    QSqlDatabase db;
    QStringListModel* listModel;

    void updateTableList();
};



#endif // DIALOGCONSOLE_H
