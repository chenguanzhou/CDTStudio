#ifndef CDTBASEITEM_H
#define CDTBASEITEM_H

#include <qgsmaplayer.h>
#include <QStandardItem>
#include <QtCore>

class CDTBaseItem:public QObject
{
    Q_OBJECT
public:    
    CDTBaseItem(QString name=QString(), QString path=QString(), CDTBaseItem* parent = 0);
    friend class CDTProjectModel;
    friend class CDTProjectItem;
    friend class CDTImageItem;
signals:
    void itemChanged();
    void childItemAdded();
public slots:
    virtual void addChildItem()=0;
private:
    QgsMapLayer* mapLayer;
    QStandardItem* rootItem;
    QStandardItem* valueItem;

};

#ifndef QT_NO_DATASTREAM
QDataStream &operator>>(QDataStream &in, CDTBaseItem &item);
QDataStream &operator<<(QDataStream &out, const CDTBaseItem &item);
#endif

#endif // CDTBASEITEM_H
