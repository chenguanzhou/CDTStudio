#ifndef CDTSAMPLEDOCKWIDGET_H
#define CDTSAMPLEDOCKWIDGET_H

#include "cdtdockwidget.h"
#include <QItemDelegate>
#include <QColorDialog>

namespace Ui {
class CDTSampleDockWidget;
}

class CDTImageLayer;
class QSqlRelationalTableModel;
class QSqlQueryModel;
class QgsMapTool;
struct QUuid;
class QSqlRecord;
class CDTMapToolSelectTrainingSamples;
class QColor;
class QColorDialog;


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

class CDTSampleDockWidget : public CDTDockWidget
{
    Q_OBJECT

public:
    explicit CDTSampleDockWidget(QWidget *parent = 0);
    ~CDTSampleDockWidget();

    bool isValid();
    QUuid currentCategoryID();

private:
    void setImageID(QUuid uuid);
    void setSegmentationID(QUuid uuid);

public slots:
    void setCurrentLayer(CDTBaseObject* layer);
    void onCurrentProjectClosed();

    void updateTable();
    void updateComboBox();
    void updateListView();
    void clear();

private slots:
    void on_actionInsert_triggered();
    void on_actionRemove_triggered();
    void on_actionRemove_All_triggered();
    void on_actionRevert_triggered();
    void on_actionSubmit_triggered();
    void onPrimeInsert(int, QSqlRecord& record);
    void on_actionEdit_triggered(bool checked);

    void on_comboBox_currentIndexChanged(int index);
    void on_toolButtonEditSample_toggled(bool checked);
    void on_toolButtonSampleRename_clicked();
    void on_toolButtonNewSample_clicked();
    void on_toolButtonRemoveSelected_clicked();
    void on_listView_clicked(const QModelIndex &index);
    void on_groupBoxSamples_toggled(bool toggled);

private:
    Ui::CDTSampleDockWidget *ui;
    QSqlRelationalTableModel *categoryModel;
    QSqlQueryModel *sampleModel;
    QAbstractItemDelegate* delegateColor;
    QAbstractItemDelegate* delegateImageID;
    QUuid   imageLayerID;

    QgsMapTool *lastMapTool;
    CDTMapToolSelectTrainingSamples* currentMapTool;
};

#endif // CDTSAMPLEDOCKWIDGET_H
