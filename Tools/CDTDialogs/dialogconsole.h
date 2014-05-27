#ifndef DIALOGCONSOLE_H
#define DIALOGCONSOLE_H

#include <QDialog>
#include <QtSql>
#include "cdtdialogs_global.h"

namespace Ui {
class DialogConsole;
}

class QStringListModel;
class QModelIndex;

class CDTDIALOGS_EXPORT DialogConsole : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConsole(QWidget *parent = 0);
    ~DialogConsole();

public slots:
    void updateDatabases();

private slots:
    void onTableSelected(const QModelIndex &index);
    void onQuery();
    void onQueryTextChanged();
    void onDatabaseChanged(QString connName);

private:
    Ui::DialogConsole *ui;
    QSqlDatabase db;
    QStringListModel* listModel;
};



#endif // DIALOGCONSOLE_H
