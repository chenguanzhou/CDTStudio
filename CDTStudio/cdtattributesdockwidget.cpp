#include "cdtattributesdockwidget.h"
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QUrl>
#include <QDebug>
#include <formattributes.h>

CDTAttributesDockWidget::CDTAttributesDockWidget(QWidget *parent) :
    QDockWidget(parent),
    _frmAttributes(new FormAttributes(this))
{

}

QToolBar *CDTAttributesDockWidget::toolBar() const
{
    return _toolBar;
}

FormAttributes *CDTAttributesDockWidget::formAttributes() const
{
    return _frmAttributes;
}

void CDTAttributesDockWidget::setCurrentDB(QUrl dbPath)
{
    QMessageBox::information(this,dbPath.scheme(),QString());
}

