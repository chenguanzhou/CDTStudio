#ifndef CDTDOCKWIDGET_H
#define CDTDOCKWIDGET_H

#include <QDockWidget>
#include "cdtbaseobject.h"
#include "cdtproject.h"

class CDTDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit CDTDockWidget(QWidget *parent = 0):QDockWidget(parent){}

signals:

    //TODO  Add Lock
public slots:
    virtual void setCurrentLayer(CDTBaseObject* layer) = 0;
    virtual void onCurrentProjectClosed() = 0;
};

#endif // CDTDOCKWIDGET_H
