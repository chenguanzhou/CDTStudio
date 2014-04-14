#ifndef CDTTRAININGSAMPLESFORM_H
#define CDTTRAININGSAMPLESFORM_H

#include <QWidget>
#include <QItemDelegate>
#include <QPainter>
#include <QColor>
#include <QColorDialog>

namespace Ui {
class CDTTrainingSamplesForm;
}
class CDTImageLayer;
class QSqlRelationalTableModel;
struct QUuid;
class QSqlRecord;

class CategoryInformation
{
public:
    CategoryInformation(QUuid uuid=QUuid(),QString name=QString(),QColor clr=QColor())
        :id(uuid),categoryName(name),color(clr){}
    QUuid id;
    QString categoryName;
    QColor color;
};

QDataStream &operator <<(QDataStream &out,const CategoryInformation &categoryInformation);
QDataStream &operator >>(QDataStream &in, CategoryInformation &categoryInformation);

typedef QList<CategoryInformation> CDTCategoryInformationList;

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

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
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
};

/*
class CDTReadOnlyDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CDTReadOnlyDelegate(QObject *parent = 0)
        :QItemDelegate(parent){}
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
    {
        return NULL;
    }
};*/

class CDTTrainingSamplesForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDTTrainingSamplesForm( QWidget *parent = 0);
    ~CDTTrainingSamplesForm();

//    QColor getColor(QString name);
//    void setImageLayer(CDTImageLayer *layer);
    void setImageID(QUuid uuid);
    bool isValid();
public slots:
    void updateTable();
    void updateComboBox();
private slots:
    void on_actionInsert_triggered();
    void on_actionRemove_triggered();
    void on_actionRemove_All_triggered();
    void on_actionRevert_triggered();
    void on_actionSubmit_triggered();
    void onPrimeInsert(int row,QSqlRecord& record);
    void on_actionEdit_triggered(bool checked);

private:
    Ui::CDTTrainingSamplesForm *ui;    
    QSqlRelationalTableModel *model;
    QAbstractItemDelegate* delegateColor;
    QAbstractItemDelegate* delegateImageID;
    QUuid   imageLayerID;
};

#endif // CDTTRAININGSAMPLESFORM_H
