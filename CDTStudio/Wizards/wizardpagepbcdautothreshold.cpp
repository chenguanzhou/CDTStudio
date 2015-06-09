#include "wizardpagepbcdautothreshold.h"
#include "ui_wizardpagepbcdautothreshold.h"
#include "cdtautothresholdinterface.h"

extern QList<CDTAutoThresholdInterface *>  autoThresholdPlugins;

WizardPagePBCDAutoThreshold::WizardPagePBCDAutoThreshold(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardPagePBCDAutoThreshold)
{
    ui->setupUi(this);
    foreach (CDTAutoThresholdInterface *pbcdPlugin, autoThresholdPlugins) {
        ui->comboBoxAutoThresholdMethod->addItem(pbcdPlugin->methodName());
    }
}

WizardPagePBCDAutoThreshold::~WizardPagePBCDAutoThreshold()
{
    delete ui;
}
