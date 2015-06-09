#include "wizardpagepbcddiff.h"
#include "ui_wizardpagepbcddiff.h"

#include "stable.h"
#include "dialogpbcdaddbandpair.h"
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"

extern QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;
extern QList<CDTPBCDMergeInterface *>      pbcdMergePlugins;

WizardPagePBCDDiff::WizardPagePBCDDiff(QUuid projectID,QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardPagePBCDDiff),
    prjID(projectID),
    modelImage(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    connect(ui->comboBoxT1Image,SIGNAL(currentIndexChanged(int)),SLOT(onT1ImageChanged(int)));
    connect(ui->comboBoxT2Image,SIGNAL(currentIndexChanged(int)),SLOT(onT2ImageChanged(int)));

    connect(ui->pushButtonAddBandPair,SIGNAL(clicked()),SLOT(onAddBandPair()));
    connect(ui->pushButtonAutoBand,SIGNAL(clicked()),SLOT(onAutoBand()));
    connect(ui->pushButtonRemoveBands,SIGNAL(clicked()),SLOT(onRemoveBands()));
    connect(ui->pushButtonRemoveAllBands,SIGNAL(clicked()),SLOT(clearBandPairs()));
    connect(ui->listWidgetBandPairs,SIGNAL(itemSelectionChanged()),SLOT(onSelectionChanged()));

    connect(ui->pushButtonGenerate,SIGNAL(clicked()),SLOT(generate()));

    modelImage->setQuery(QString("select name,id,path from imagelayer where projectid  = '%1'").arg(prjID.toString()),
                    QSqlDatabase::database("category"));
    ui->comboBoxT1Image->setModel(modelImage);
    ui->comboBoxT2Image->setModel(modelImage);
    ui->comboBoxT2Image->setCurrentIndex(1);

    foreach (CDTPBCDDiffInterface *pbcdPlugin, pbcdDiffPlugins) {
        ui->comboBoxDiffMethod->addItem(pbcdPlugin->methodName());
    }

    foreach (CDTPBCDMergeInterface *pbcdPlugin, pbcdMergePlugins) {
        ui->comboBoxMergeMethod->addItem(pbcdPlugin->methodName());
    }

    ui->pushButtonAutoBand->click();
}

WizardPagePBCDDiff::~WizardPagePBCDDiff()
{
    delete ui;
}


void WizardPagePBCDDiff::onT1ImageChanged(int row)
{
    QString path = modelImage->data(modelImage->index(row,2)).toString();
    ui->labelT1Path->setText(path);

    clearBandPairs();
}

void WizardPagePBCDDiff::onT2ImageChanged(int row)
{
    QString path = modelImage->data(modelImage->index(row,2)).toString();
    ui->labelT2Path->setText(path);

    clearBandPairs();
}

void WizardPagePBCDDiff::onAddBandPair()
{
    QString newPair = DialogPBCDAddBandPair::getNewPair(ui->labelT1Path->text(),ui->labelT2Path->text());
    if (newPair.isEmpty())
        return;
    if (ui->listWidgetBandPairs->findItems(newPair,Qt::MatchExactly).size()!=0)
        return;
    ui->listWidgetBandPairs->addItem(newPair);
    updatePushbuttonRemoveAll();
}

void WizardPagePBCDDiff::onAutoBand()
{
    QgsRasterLayer layer1(ui->labelT1Path->text());
    QgsRasterLayer layer2(ui->labelT2Path->text());
    int bandCount1 = layer1.bandCount();
    int bandCount2 = layer2.bandCount();

    int minCount = qMin(bandCount1,bandCount2);
    this->clearBandPairs();
    for (int i=0;i<minCount;++i)
    {
        ui->listWidgetBandPairs->addItem(QString("band%1->band%1").arg(i+1));
    }
//    ui->listWidgetBandPairs->addItem("ave->ave");
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void WizardPagePBCDDiff::onRemoveBands()
{
    auto slectedItems = ui->listWidgetBandPairs->selectedItems();
    foreach (QListWidgetItem *item, slectedItems) {
        ui->listWidgetBandPairs->removeItemWidget(item);
        delete item;
    }
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void WizardPagePBCDDiff::onSelectionChanged()
{
    ui->pushButtonRemoveBands->setEnabled(ui->listWidgetBandPairs->selectedItems().size()!=0);
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void WizardPagePBCDDiff::updatePushbuttonRemoveAll()
{
    ui->pushButtonRemoveAllBands->setEnabled(ui->listWidgetBandPairs->count()!=0);
}

void WizardPagePBCDDiff::clearBandPairs()
{
    ui->listWidgetBandPairs->clear();
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void WizardPagePBCDDiff::updateGroupBoxMerge()
{
    ui->groupBoxMerge->setEnabled(!ui->listWidgetBandPairs->count()<=1);
}

void WizardPagePBCDDiff::generate()
{

}
