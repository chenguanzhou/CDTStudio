#ifndef CDTLAYERINFOWIDGET_H
#define CDTLAYERINFOWIDGET_H

#include "cdtdockwidget.h"
#include "log4qt/logger.h"
class QPropertyEditorWidget;

class CDTLayerInfoWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTLayerInfoWidget(QWidget *parent = 0);

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

private:
    QPropertyEditorWidget *propertyWidget;
};

#endif // CDTLAYERINFOWIDGET_H
