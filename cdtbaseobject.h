#ifndef CDTBASEOBJECT_H
#define CDTBASEOBJECT_H

#include <QObject>

class CDTBaseObject : public QObject
{
    Q_OBJECT
public:
    explicit CDTBaseObject(QObject *parent = 0);

    virtual void onContextMenuRequest(QWidget *parent) = 0;
signals:

public slots:

};

#endif // CDTBASEOBJECT_H
