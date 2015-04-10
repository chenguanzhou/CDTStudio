#ifndef DIALOGVALIDATIONPOINTS_H
#define DIALOGVALIDATIONPOINTS_H

#include <QDialog>

class QTableView;
class QSqlRelationalTableModel;
class QItemSelection;
class DialogValidationPoints : public QDialog
{
    Q_OBJECT
public:
    explicit DialogValidationPoints(const QString validationID,QWidget *parent = 0);
    ~DialogValidationPoints();
signals:

protected slots:
    void onSelectionChanged(const QItemSelection &items);

private:
    void closeEvent(QCloseEvent *e);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    QTableView *tableView;
    QSqlRelationalTableModel *model;
};

#endif // DIALOGVALIDATIONPOINTS_H
