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
    void updateDatabases();

private slots:
    void onQuery();
    void onQueryTextChanged();
    void onDatabaseChanged(QString connName);
    void onContextMenu(QPoint pt);
    void deleteDataInCurrentTable();
    void onSelectionChanged(QItemSelection selection);

private:
    void updateCurrentTable(const QModelIndex &index);

    Ui::DialogConsole *ui;
    QSqlDatabase db;
    QStringListModel* listModel;
};



#endif // DIALOGCONSOLE_H
