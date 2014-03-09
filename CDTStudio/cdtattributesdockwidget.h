#ifndef CDTATTRIBUTESDOCKWIDGET_H
#define CDTATTRIBUTESDOCKWIDGET_H

#include <QDockWidget>
class QToolBar;

class CDTAttributesDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit CDTAttributesDockWidget(QWidget *parent = 0);

    QToolBar *toolBar ()const;
signals:

public slots:

private:
    QToolBar *_toolBar;

};

#endif // CDTATTRIBUTESDOCKWIDGET_H
