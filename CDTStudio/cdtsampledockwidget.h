#ifndef CDTSAMPLEDOCKWIDGET_H
#define CDTSAMPLEDOCKWIDGET_H

#include <QDockWidget>

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

class CDTSampleDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CDTSampleDockWidget(QWidget *parent = 0);
    ~CDTSampleDockWidget();

    void setImageID(QUuid uuid);
    void setSegmentationID(QUuid uuid);
    bool isValid();
    QUuid currentCategoryID();

signals:
    void segmentationChanged();
    void imageChanged();

public slots:
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
    void onPrimeInsert(int row,QSqlRecord& record);
    void on_actionEdit_triggered(bool checked);

    void on_comboBox_currentIndexChanged(int index);
    void on_toolButtonRefresh_clicked();
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
