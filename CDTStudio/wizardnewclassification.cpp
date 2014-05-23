#include "wizardnewclassification.h"
#include "ui_wizardnewclassification.h"

WizardNewClassification::WizardNewClassification(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardNewClassification)
{
    ui->setupUi(this);
}

WizardNewClassification::~WizardNewClassification()
{
    delete ui;
}
