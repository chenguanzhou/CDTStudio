#include "wizardvectorchangedetection.h"
#include "ui_wizardvectorchangedetection.h"

WizardVectorChangeDetection::WizardVectorChangeDetection(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardVectorChangeDetection)
{
    ui->setupUi(this);
}

WizardVectorChangeDetection::~WizardVectorChangeDetection()
{
    delete ui;
}
