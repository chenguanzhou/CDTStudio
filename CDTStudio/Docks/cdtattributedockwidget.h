#ifndef CDTATTRIBUTEDOCKWIDGET_H
#define CDTATTRIBUTEDOCKWIDGET_H

#include "cdtdockwidget.h"
#include "dialogdbconnection.h"
#include "log4qt/logger.h"

struct QUuid;
class QPainter;
class QWidget;
class QTabWidget;
class QAbstractTableModel;
class CDTSegmentationLayer;


namespace Ui {
class CDTAttributeDockWidget;
}

class CDTObjectIDDelegate:public QItemDelegate
{
    Q_OBJECT
public:
    CDTObjectIDDelegate ( QObject * parent = 0 ):QItemDelegate(parent){}
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        QString text = index.data(Qt::DisplayRole).toString();
        painter->save();
        QRect cellRect = option.rect;
        painter->setPen(QColor(255,0,0));

        QFont font = index.data(Qt::FontRole).value<QFont>();
        font.setBold(true);
        painter->setFont(font);

        painter->drawText(cellRect, Qt::AlignHCenter|Qt::AlignVCenter, text);
        painter->restore();
    }
};

class CDTAttributeDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTAttributeDockWidget(QWidget *parent = 0);
    ~CDTAttributeDockWidget();

//    CDTSegmentationLayer *segmLayer()const;

public slots:    
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

    void clearTables();
    void clear();

private slots:
    void setTableModels(QList<QAbstractTableModel*> models);
    void onItemClicked(QModelIndex index);
    void onActionExportCurrentTable();
    void onActionExportAllTables();
private:
    CDTDatabaseConnInfo     dbConnInfo;
//    CDTSegmentationLayer*   segmentationLayer;
    QUuid segmentationID;
    QTabWidget *tabWidget;
};

#endif // CDTATTRIBUTEDOCKWIDGET_H
