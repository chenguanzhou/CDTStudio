#include "wizardpagepbcddiff.h"
#include "ui_wizardpagepbcddiff.h"

#include "stable.h"
#include "cdtimagelayer.h"
#include "dialogpbcdaddbandpair.h"
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"
#include "cdtpbcddiff.h"

extern QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;
extern QList<CDTPBCDMergeInterface *>      pbcdMergePlugins;

WizardPagePBCDDiff::WizardPagePBCDDiff(QUuid projectID,QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardPagePBCDDiff),
    prjID(projectID),
    modelImage(new QSqlQueryModel(this)),
    isGenerated(false)
{
    ui->setupUi(this);

    connect(ui->comboBoxT1Image,SIGNAL(currentIndexChanged(int)),SLOT(onT1ImageChanged(int)));
    connect(ui->comboBoxT2Image,SIGNAL(currentIndexChanged(int)),SLOT(onT2ImageChanged(int)));
    connect(ui->comboBoxT1Image,SIGNAL(currentIndexChanged(int)),SLOT(setUnGenerated()));
    connect(ui->comboBoxT2Image,SIGNAL(currentIndexChanged(int)),SLOT(setUnGenerated()));

    connect(ui->pushButtonAddBandPair,SIGNAL(clicked()),SLOT(onAddBandPair()));
    connect(ui->pushButtonAutoBand,SIGNAL(clicked()),SLOT(onAutoBand()));
    connect(ui->pushButtonRemoveBands,SIGNAL(clicked()),SLOT(onRemoveBands()));
    connect(ui->pushButtonRemoveAllBands,SIGNAL(clicked()),SLOT(clearBandPairs()));
    connect(ui->listWidgetBandPairs,SIGNAL(itemSelectionChanged()),SLOT(onSelectionChanged()));
    connect(ui->pushButtonAddBandPair,SIGNAL(clicked()),SLOT(setUnGenerated()));
    connect(ui->pushButtonAutoBand,SIGNAL(clicked()),SLOT(setUnGenerated()));
    connect(ui->pushButtonRemoveBands,SIGNAL(clicked()),SLOT(setUnGenerated()));
    connect(ui->pushButtonRemoveAllBands,SIGNAL(clicked()),SLOT(setUnGenerated()));
    connect(ui->listWidgetBandPairs,SIGNAL(itemSelectionChanged()),SLOT(setUnGenerated()));

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

bool WizardPagePBCDDiff::validatePage()
{
    return isGenerated;
}


void WizardPagePBCDDiff::onT1ImageChanged(int row)
{
    QString id = modelImage->data(modelImage->index(row,1)).toString();
    CDTImageLayer* layer = CDTImageLayer::getLayer(QUuid(id));
    ui->labelT1Path->setText(layer->absolutPath());

    clearBandPairs();
}

void WizardPagePBCDDiff::onT2ImageChanged(int row)
{
    QString id = modelImage->data(modelImage->index(row,1)).toString();
    CDTImageLayer* layer = CDTImageLayer::getLayer(QUuid(id));
    ui->labelT2Path->setText(layer->absolutPath());

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
    updateGroupBoxMerge();
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
        ui->listWidgetBandPairs->addItem(QString("band%1<->band%1").arg(i+1));
    }
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
    ui->comboBoxMergeMethod->setEnabled(ui->listWidgetBandPairs->count()>1);
}

void WizardPagePBCDDiff::setUnGenerated()
{
    isGenerated = false;
    //Remove images
}

void WizardPagePBCDDiff::generationFinished()
{
    this->setEnabled(true);
    CDTPBCDDiff *thread = qobject_cast<CDTPBCDDiff *>(sender());
    if (thread==NULL)
        return;

    if (!thread->isCompleted())//Failed
        return;

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
        wizard->setProperty("FloatImage",thread->outputPath);
        wizard->setProperty("NumOfThresholds",thread->numOfThresholds);
        isGenerated = true;
    }
    else
    {
        qDebug()<<"Failed!";
    }

//    setField("Float Image",outputPath);
//    setField("Num of Thresholds",1);
}

void WizardPagePBCDDiff::showWarningMessage(QString msg)
{
    QMessageBox::warning(NULL,tr("Warning"),msg);
}

void WizardPagePBCDDiff::generate()
{
    QString t1Path = ui->labelT1Path->text();
    QString t2Path = ui->labelT2Path->text();
    if (t1Path.isEmpty() || t2Path.isEmpty())
    {
        QMessageBox::critical(this,tr("Error"),tr("Image path is empty!"));
        return;
    }

    QList<QPair<uint,uint> > bandPairs;
    for (int i=0;i<ui->listWidgetBandPairs->count();++i)
    {
        QStringList pair = ui->listWidgetBandPairs->item(i)->text().split("<->");
        if (pair.count()<2)
        {
            QMessageBox::critical(this,tr("Error"),tr("One of image band pair is invalid!"));
            return;
        }
        uint id1 = (pair[0]=="ave")?0:pair[0].remove("band").toUInt();
        uint id2 = (pair[1]=="ave")?0:pair[1].remove("band").toUInt();

        bandPairs.push_back(qMakePair(id1,id2));
    }

    if (ui->comboBoxDiffMethod->count()==0)
    {
        QMessageBox::critical(this,tr("Error"),tr("No Diff Plugin Found!"));
        return;
    }

    if (bandPairs.count()>1 && ui->comboBoxMergeMethod->count()==0)
    {
        QMessageBox::critical(this,tr("Error"),tr("No Merge Plugin Found!"));
        return;
    }

    CDTPBCDDiffInterface *diffPlugin;
    foreach (CDTPBCDDiffInterface *plugin, pbcdDiffPlugins) {
        if (plugin->methodName()==ui->comboBoxDiffMethod->currentText())
        {
            diffPlugin = plugin;
        }
    }

    CDTPBCDMergeInterface *mergePlugin;
    foreach (CDTPBCDMergeInterface *plugin, pbcdMergePlugins) {
        if (plugin->methodName()==ui->comboBoxMergeMethod->currentText())
        {
            mergePlugin = plugin;
        }
    }

    this->setEnabled(false);
    CDTPBCDDiff *thread = new CDTPBCDDiff(t1Path,t2Path,bandPairs,diffPlugin,mergePlugin,this);
    connect(thread,SIGNAL(finished()),SLOT(generationFinished()));
    connect(thread,SIGNAL(showWarningMessage(QString)),SLOT(showWarningMessage(QString)));
    connect(thread,SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    connect(thread,SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(thread,SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));

    thread->start();
}
