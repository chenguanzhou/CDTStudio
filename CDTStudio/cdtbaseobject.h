#ifndef CDTBASEOBJECT_H
#define CDTBASEOBJECT_H

#include <QObject>
class CDTProjectTreeItem;
class QStandardItem;

class CDTBaseObject : public QObject
{
    Q_OBJECT
public:
    explicit CDTBaseObject(QObject *parent = 0);

    virtual void onContextMenuRequest(QWidget *parent) = 0;
    QList<QStandardItem *> standardItems()const;
signals:

public slots:

protected:
    CDTProjectTreeItem* keyItem;
    CDTProjectTreeItem* valueItem;
};

#endif // CDTBASEOBJECT_H
