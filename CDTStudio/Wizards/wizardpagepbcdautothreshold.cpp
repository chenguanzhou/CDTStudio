#include "wizardpagepbcdautothreshold.h"
#include "ui_wizardpagepbcdautothreshold.h"
#include "stable.h"
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

    if (wizard==NULL)
        return;

    QString imagePath = wizard->property("FloatImage").toString();
    int numOfThresholds = wizard->property("NumOfThresholds").toInt();
    ui->labelNeg->setEnabled(numOfThresholds!=1);
    ui->doubleSpinBoxNegT->setEnabled(numOfThresholds!=1);

    GDALDataset *poDS = (GDALDataset *)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    if (poDS==NULL)
    {
        qDebug()<<"Null dataset!";
        return;
    }

    GDALRasterBand *poBand = poDS->GetRasterBand(1);

    double maxVal = poBand->GetMaximum();
    double minVal = poBand->GetMinimum();

    ui->doubleSpinBoxNegT->setMinimum(minVal);
    ui->doubleSpinBoxNegT->setMaximum(maxVal);
    ui->doubleSpinBoxPosT->setMinimum(minVal);
    ui->doubleSpinBoxPosT->setMaximum(maxVal);


}
