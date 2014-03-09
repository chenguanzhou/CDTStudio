#include "cdtattributesdockwidget.h"
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QLayout>
#include <QVBoxLayout>

CDTAttributesDockWidget::CDTAttributesDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _toolBar(new QToolBar(tr("Attributes"),this))
{
    QAction *actionGenerateAttributes = new QAction(tr("Generate Attributes"),_toolBar);
    _toolBar->addAction(actionGenerateAttributes);
    _toolBar->setIconSize(QSize(32,32));
}

QToolBar *CDTAttributesDockWidget::toolBar() const
{
    return _toolBar;
}
