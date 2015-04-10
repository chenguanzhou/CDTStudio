#include "dialogpbcdbinary.h"
#include "ui_dialogpbcdbinary.h"
#include "stable.h"
#include "cdtapplication.h"
#include "mainwindow.h"
#include "cdttaskdockwidget.h"

#include "dialogpbcdaddbandpair.h"
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"
#include "cdtautothresholdinterface.h"

extern QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;
extern QList<CDTPBCDMergeInterface *>      pbcdMergePlugins;
extern QList<CDTAutoThresholdInterface *>  autoThresholdPlugins;

DialogPBCDBinary::DialogPBCDBinary(QUuid projectID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPBCDBinary),
    prjID(projectID),
    modelImage(new QSqlQueryModel(this)),
    isDoubleThreshold(false),
    reply(NULL)
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

    connect(this,SIGNAL(accepted()),SLOT(generateXML()));

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

    foreach (CDTAutoThresholdInterface *pbcdPlugin, autoThresholdPlugins) {
        ui->comboBoxAutoThresholdMethod->addItem(pbcdPlugin->methodName());
    }

    ui->pushButtonAutoBand->click();

}

DialogPBCDBinary::~DialogPBCDBinary()
{
    delete ui;
}

CDTTaskReply* DialogPBCDBinary::startBinaryPBCD(QUuid prjID)
{
    DialogPBCDBinary dlg(prjID);
    dlg.exec();
    return dlg.reply;
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
    updateGroupBoxMerge();
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
//    ui->listWidgetBandPairs->addItem("ave->ave");
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
    updateDoubleThreshold();
}

void DialogPBCDBinary::onRemoveBands()
{
    auto slectedItems = ui->listWidgetBandPairs->selectedItems();
    foreach (QListWidgetItem *item, slectedItems) {
        ui->listWidgetBandPairs->removeItemWidget(item);
        delete item;
    }
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void DialogPBCDBinary::onSelectionChanged()
{
    ui->pushButtonRemoveBands->setEnabled(ui->listWidgetBandPairs->selectedItems().size()!=0);
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void DialogPBCDBinary::updatePushbuttonRemoveAll()
{
    ui->pushButtonRemoveAllBands->setEnabled(ui->listWidgetBandPairs->count()!=0);
}

void DialogPBCDBinary::clearBandPairs()
{
    ui->listWidgetBandPairs->clear();
    updatePushbuttonRemoveAll();
    updateGroupBoxMerge();
}

void DialogPBCDBinary::updateGroupBoxMerge()
{
    isDoubleThreshold = ui->listWidgetBandPairs->count()<=1;
    ui->groupBoxMerge->setEnabled(!isDoubleThreshold);
    updateDoubleThreshold();
}

void DialogPBCDBinary::updateDoubleThreshold()
{
    ui->doubleSpinBoxMaxT->setEnabled(ui->groupBoxManualThreshold->isChecked() & isDoubleThreshold);
}

void DialogPBCDBinary::onAutoThresholdToggled(bool toggled)
{
    ui->groupBoxManualThreshold->blockSignals(true);
    ui->groupBoxManualThreshold->setChecked(!toggled);
    ui->groupBoxManualThreshold->blockSignals(false);    
    updateDoubleThreshold();
}

void DialogPBCDBinary::onManualThresholdToggled(bool toggled)
{
    ui->groupBoxAutoThreshold->blockSignals(true);
    ui->groupBoxAutoThreshold->setChecked(!toggled);    
    ui->groupBoxAutoThreshold->blockSignals(false);    
    updateDoubleThreshold();
}

void DialogPBCDBinary::generateXML()
{
    QDomDocument doc("PBCDBinaryParams");
    QDomElement root = doc.createElement("CDTTask");
    doc.appendChild(root);
    QDomElement createTask = doc.createElement("create");
    root.appendChild(createTask);

    QString id = QUuid::createUuid().toString();
    const QString name = "PBCD_Binary";
    createTask.setAttribute("name",name);
    createTask.setAttribute("id",id);
    createTask.setAttribute("time",QDateTime::currentDateTime().toString());

    QDomElement params = doc.createElement("params");
    createTask.appendChild(params);

    QDomElement iamges = doc.createElement("images");
    QDomElement bands = doc.createElement("bands");
    QDomElement radiometric_correction = doc.createElement("radiometric_correction");
    QDomElement diff_method = doc.createElement("diff_method");
    QDomElement merge_method = doc.createElement("merge_method");
    QDomElement threshold = doc.createElement("threshold");
    params.appendChild(iamges);
    params.appendChild(bands);
    params.appendChild(radiometric_correction);
    params.appendChild(diff_method);
    params.appendChild(merge_method);
    params.appendChild(threshold);

    QDomElement t1 = doc.createElement("t1");
    QDomElement t2 = doc.createElement("t2");
    t1.setAttribute("path",ui->labelT1Path->text());
    t2.setAttribute("path",ui->labelT2Path->text());
    iamges.appendChild(t1);
    iamges.appendChild(t2);

    for (int i=0;i<ui->listWidgetBandPairs->count();++i)
    {
        QDomElement pair = doc.createElement("band_pair");
        QDomText text = doc.createTextNode(ui->listWidgetBandPairs->item(i)->text());
        pair.appendChild(text);
        bands.appendChild(pair);
    }

    if (ui->groupBoxRadiometricCorrection->isChecked())
    {
        radiometric_correction.setAttribute("valid","true");
        QDomText text = doc.createTextNode(ui->comboBoxRadiometricCorrection->currentText());
        radiometric_correction.appendChild(text);
    }
    else
        radiometric_correction.setAttribute("valid","false");

    diff_method.setAttribute("name",ui->comboBoxDiffMethod->currentText());
//    diff_method.appendChild() //TODO: params

    if (ui->groupBoxMerge->isEnabled())
        merge_method.setAttribute("name",ui->comboBoxMergeMethod->currentText());
    else
        merge_method.setAttribute("name","null");

    if (ui->groupBoxAutoThreshold->isChecked())
    {
        threshold.setAttribute("type","auto");
        QDomText text = doc.createTextNode(ui->comboBoxAutoThresholdMethod->currentText());
        threshold.appendChild(text);
    }
    else
    {
        threshold.setAttribute("type","manual");
        QString textThreshold;
        if (ui->doubleSpinBoxMaxT->isEnabled())
            textThreshold = ui->doubleSpinBoxMinT->text() + ";" + ui->doubleSpinBoxMaxT->text();
        else
            textThreshold = ui->doubleSpinBoxMinT->text();
        QDomText text = doc.createTextNode(textThreshold);
        threshold.appendChild(text);
    }

    qApp->sendTask( doc.toByteArray() ) ;
    reply = MainWindow::getTaskDockWIdget()->appendNewTask(id,name,prjID.toString());

    reply->setProperty("name",name);
    reply->setProperty("image_t1",modelImage->data(modelImage->index(ui->comboBoxT1Image->currentIndex(),1)));
    reply->setProperty("image_t2",modelImage->data(modelImage->index(ui->comboBoxT2Image->currentIndex(),1)));

//    connect(reply,SIGNAL(completed(QByteArray)),this,SLOT(onCompleted(QByteArray)));
}

//void DialogPBCDBinary::onCompleted(QByteArray result)
//{
//    QDataStream in(result);
//    QList<double> thresholds;
//    QString diffPath;
//    in>>thresholds>>diffPath;
//    qDebug()<<"thresolds: "<<thresholds<<"diff path: "<<diffPath;
//    deleteLater();
//}
