#ifndef CDTVALIDATIONSAMPLEDOCKWIDGET_H
#define CDTVALIDATIONSAMPLEDOCKWIDGET_H

#include "cdtdockwidget.h"
class QGroupBox;
class QToolBar;
class QListView;
class CDTValidationSampleDockWidget : public CDTDockWidget
{
    Q_OBJECT
public:
    explicit CDTValidationSampleDockWidget(QWidget *parent = 0);

signals:

public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onCurrentProjectClosed();
private:
    QGroupBox   *groupBox;
    QToolBar    *toolbar;
    QListView   *listView;

};

#endif // CDTVALIDATIONSAMPLEDOCKWIDGET_H
