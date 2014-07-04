#include "cdtextractiondockwidget.h"
#include "ui_cdtextractiondockwidget.h"

CDTExtractionDockWidget::CDTExtractionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDTExtractionDockWidget)
{
    ui->setupUi(this);
}

CDTExtractionDockWidget::~CDTExtractionDockWidget()
{
    delete ui;
}
