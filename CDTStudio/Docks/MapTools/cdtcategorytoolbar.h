#ifndef CDTCATEGORYTOOLBAR_H
#define CDTCATEGORYTOOLBAR_H

#include <QToolBar>

class CDTCategoryToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit CDTCategoryToolBar(const QString &title, QWidget *parent = 0);

signals:

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // CDTCATEGORYTOOLBAR_H
