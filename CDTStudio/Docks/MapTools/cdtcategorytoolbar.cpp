#include "cdtcategorytoolbar.h"

CDTCategoryToolBar::CDTCategoryToolBar(const QString &title, QWidget *parent) :
    QToolBar(title,parent)
{
    qDebug()<<"CDTCategoryToolBar";
}

void CDTCategoryToolBar::enterEvent(QEvent *)
{
    this->setWindowOpacity(1);
}

void CDTCategoryToolBar::leaveEvent(QEvent *)
{
    this->setWindowOpacity(0.7);
}
