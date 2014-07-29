#ifndef CDTLAYERINFOWIDGET_H
#define CDTLAYERINFOWIDGET_H

#include "cdtdockwidget.h"
class QPropertyEditorWidget;

class CDTLayerInfoWidget : public CDTDockWidget
{
    Q_OBJECT
public:
    explicit CDTLayerInfoWidget(QWidget *parent = 0);

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();

private:
    QPropertyEditorWidget *propertyWidget;
};

#endif // CDTLAYERINFOWIDGET_H
