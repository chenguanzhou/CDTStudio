#ifndef CDTVALIDATIONSAMPLEDOCKWIDGET_H
#define CDTVALIDATIONSAMPLEDOCKWIDGET_H

#include "cdtdockwidget.h"
#include "log4qt/logger.h"
class QGroupBox;
class QToolBar;
class QListView;
class CDTValidationSampleDockWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTValidationSampleDockWidget(QWidget *parent = 0);

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();
private:
    QGroupBox   *groupBox;
    QToolBar    *toolbar;
    QListView   *listView;

};

#endif // CDTVALIDATIONSAMPLEDOCKWIDGET_H
