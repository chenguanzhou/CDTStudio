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

void WizardPagePBCDAutoThreshold::initializePage()
{
    QObject *prt = this;
    QWizard *wizard = NULL;
    while(prt)
    {
        wizard = qobject_cast<QWizard *>(prt);
        if (wizard!=NULL)
            break;
        prt = prt->parent();
    }

    if (wizard)
    {
        QString imagePath = wizard->property("FloatImage").toString();
        qDebug()<<"WizardPagePBCDAutoThreshold"<<imagePath;
    }
    else
        qDebug()<<"Nothing";
}
