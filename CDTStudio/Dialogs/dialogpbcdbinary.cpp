#include "dialogpbcdbinary.h"
#include "ui_dialogpbcdbinary.h"
#include "stable.h"

#include "dialogpbcdaddbandpair.h"

DialogPBCDBinary::DialogPBCDBinary(QUuid projectID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPBCDBinary),
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

    connect(ui->groupBoxAutoThreshold,SIGNAL(toggled(bool)),SLOT(onAutoThresholdToggled(bool)));
    connect(ui->groupBoxManualThreshold,SIGNAL(toggled(bool)),SLOT(onManualThresholdToggled(bool)));

    modelImage->setQuery(QString("select name,id,path from imagelayer where projectid  = '%1'").arg(prjID.toString()),
                    QSqlDatabase::database("category"));
    ui->comboBoxT1Image->setModel(modelImage);
    ui->comboBoxT2Image->setModel(modelImage);
    ui->comboBoxT2Image->setCurrentIndex(1);


}

DialogPBCDBinary::~DialogPBCDBinary()
{
    delete ui;
}

void DialogPBCDBinary::onT1ImageChanged(int row)
{
    QString path = modelImage->data(modelImage->index(row,2)).toString();
    ui->labelT1Path->setText(path);

    clearBandPairs();
}

void DialogPBCDBinary::onT2ImageChanged(int row)
{
    QString path = modelImage->data(modelImage->index(row,2)).toString();
    ui->labelT2Path->setText(path);

    clearBandPairs();
}

void DialogPBCDBinary::onAddBandPair()
{
    QString newPair = DialogPBCDAddBandPair::getNewPair(ui->labelT1Path->text(),ui->labelT2Path->text());
    if (newPair.isEmpty())
        return;
    if (ui->listWidgetBandPairs->findItems(newPair,Qt::MatchExactly).size()!=0)
        return;
    ui->listWidgetBandPairs->addItem(newPair);
    updatePushbuttonRemoveAll();
}

void DialogPBCDBinary::onAutoBand()
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
    ui->listWidgetBandPairs->addItem("ave->ave");
    updatePushbuttonRemoveAll();
}

void DialogPBCDBinary::onRemoveBands()
{
    auto slectedItems = ui->listWidgetBandPairs->selectedItems();
    foreach (QListWidgetItem *item, slectedItems) {
        ui->listWidgetBandPairs->removeItemWidget(item);
        delete item;
    }
    updatePushbuttonRemoveAll();
}

void DialogPBCDBinary::onSelectionChanged()
{
    ui->pushButtonRemoveBands->setEnabled(ui->listWidgetBandPairs->selectedItems().size()!=0);
    updatePushbuttonRemoveAll();
}

void DialogPBCDBinary::updatePushbuttonRemoveAll()
{
    ui->pushButtonRemoveAllBands->setEnabled(ui->listWidgetBandPairs->count()!=0);
}

void DialogPBCDBinary::clearBandPairs()
{
    ui->listWidgetBandPairs->clear();
    updatePushbuttonRemoveAll();
}

void DialogPBCDBinary::onAutoThresholdToggled(bool toggled)
{
    ui->groupBoxManualThreshold->blockSignals(true);
    ui->groupBoxManualThreshold->setChecked(!toggled);
    ui->groupBoxManualThreshold->blockSignals(false);
}

void DialogPBCDBinary::onManualThresholdToggled(bool toggled)
{
    ui->groupBoxAutoThreshold->blockSignals(true);
    ui->groupBoxAutoThreshold->setChecked(!toggled);
    ui->groupBoxAutoThreshold->blockSignals(false);
}
