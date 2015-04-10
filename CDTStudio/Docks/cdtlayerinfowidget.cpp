#include "cdtlayerinfowidget.h"
#include "QPropertyEditorWidget.h"
#include <QVBoxLayout>

CDTLayerInfoWidget::CDTLayerInfoWidget(QWidget *parent) :
    CDTDockWidget(parent),
    propertyWidget(new QPropertyEditorWidget(this))
{
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(widget);
    vbox->addWidget(propertyWidget);
    propertyWidget->setAlternatingRowColors(true);
    widget->setLayout(vbox);
    this->setWidget(widget);

    this->setWindowTitle(tr("Layer Infomation"));
    logger()->info("Constructed");
}

void CDTLayerInfoWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    propertyWidget->setObject(layer);
    connect(layer,SIGNAL(destroyed()),SLOT(onDockClear()));
//    this->setVisible(true);
    this->raise();
    logger()->info("Layer %1 is selected",layer->metaObject()->className());
}

void CDTLayerInfoWidget::onDockClear()
{
    propertyWidget->clear();
//    this->setVisible(false);
}
