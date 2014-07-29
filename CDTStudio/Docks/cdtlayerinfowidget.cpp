#include "cdtlayerinfowidget.h"
#include "QPropertyEditorWidget.h"
#include <QVBoxLayout>

CDTLayerInfoWidget::CDTLayerInfoWidget(QWidget *parent) :
    CDTDockWidget(parent),
    propertyWidget(new QPropertyEditorWidget(this))
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(propertyWidget);
    widget->setLayout(vbox);
    this->setWidget(widget);

    this->setWindowTitle(tr("Layer Infomation"));
}

void CDTLayerInfoWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    propertyWidget->setObject(layer);
}

void CDTLayerInfoWidget::onCurrentProjectClosed()
{
    propertyWidget->clear();
}
