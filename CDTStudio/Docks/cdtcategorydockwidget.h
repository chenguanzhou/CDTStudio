#ifndef CDTCATEGORYDOCKWIDGET_H
#define CDTCATEGORYDOCKWIDGET_H

#include <QItemDelegate>
#include "cdtdockwidget.h"
#include "log4qt/logger.h"

class QTableView;
class QSqlTableModel;
class CDTCategoryDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTCategoryDockWidget(QWidget *parent = 0);

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();
private slots:
    void updateImageID(QUuid id);
//    void updateTable();
    void on_actionInsert_triggered();
    void on_actionRemove_triggered();
    void on_actionRemove_All_triggered();
    void on_actionRevert_triggered();
    void on_actionSubmit_triggered();
//    void onPrimeInsert(int, QSqlRecord& record);
//    void on_actionEdit_triggered(bool checked);
    void importCategories();
    void exportCategories();
private:
    QTableView      *tableView;
    QSqlTableModel  *categoryModel;
    QItemDelegate   *delegateColor;
    QUuid   imageLayerID;

    QAction *actionEdit;
    QAction *actionRevert;
    QAction *actionSubmit;
    QAction *actionInsert;
    QAction *actionRemove;
    QAction *actionRemove_All;
    QAction *actionImportCategoroies;
    QAction *actionExportCategoroies;
};

class CDTCategoryDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CDTCategoryDelegate(QObject *parent = 0)
        :QItemDelegate(parent){}

    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        QColor color = index.model()->data(index).value<QColor>() ;
        painter->setBrush(QBrush(color));
        painter->setPen(QPen(QColor(Qt::transparent)));
        painter->drawRect(option.rect);
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const
    {
        QColorDialog *dlg = new QColorDialog(parent);
        return dlg;
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QColor color = QColor(index.model()->data(index).toString()) ;
        QColorDialog *dlg = static_cast<QColorDialog*>(editor);
        dlg->setCurrentColor(color);
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const
    {
        QColorDialog *dlg = static_cast<QColorDialog*>(editor);
        model->setData(index,dlg->currentColor()) ;
    }
    void updateEditorGeometry(QWidget * editor,
                              const QStyleOptionViewItem & ,
                              const QModelIndex & ) const
    {
        QRect scr = QApplication::desktop()->screenGeometry();
        editor->move( scr.center() - editor->mapToGlobal( editor->rect().center()) );
    }
};

#endif // CDTCATEGORYDOCKWIDGET_H
