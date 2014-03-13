#ifndef CDTATTRIBUTESDOCKWIDGET_H
#define CDTATTRIBUTESDOCKWIDGET_H

#include <QDockWidget>
class QUrl;
class QToolBar;
class FormAttributes;

class CDTAttributesDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit CDTAttributesDockWidget(QWidget *parent = 0);


    FormAttributes *formAttributes()const;
    void setCurrentDB(QUrl dbPath);

signals:

public slots:

private:
    FormAttributes* _frmAttributes;
};

#endif // CDTATTRIBUTESDOCKWIDGET_H
