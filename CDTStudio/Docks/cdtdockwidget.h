#ifndef CDTDOCKWIDGET_H
#define CDTDOCKWIDGET_H

#include <QDockWidget>
#include "cdtbaselayer.h"
#include "cdtprojectlayer.h"

class CDTDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit CDTDockWidget(QWidget *parent = 0):QDockWidget(parent){}

signals:

public slots:
    virtual void setCurrentLayer(CDTBaseLayer* layer) = 0;
    virtual void onDockClear() = 0;
};

#endif // CDTDOCKWIDGET_H
