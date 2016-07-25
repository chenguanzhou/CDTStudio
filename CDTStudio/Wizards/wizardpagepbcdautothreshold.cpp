#include "wizardpagepbcdautothreshold.h"
#include "ui_wizardpagepbcdautothreshold.h"
#include "stable.h"
#include "cdtautothresholdinterface.h"
#include "cdtpbcdhelper.h"

extern QList<CDTAutoThresholdInterface *>  autoThresholdPlugins;

WizardPagePBCDAutoThreshold::WizardPagePBCDAutoThreshold(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardPagePBCDAutoThreshold),
    isGenerated(false)
{
    ui->setupUi(this);
    foreach (CDTAutoThresholdInterface *pbcdPlugin, autoThresholdPlugins) {
        ui->comboBoxAutoThresholdMethod->addItem(pbcdPlugin->methodName());
    }

    connect(ui->pushButtonThreshold,SIGNAL(clicked()),SLOT(applyAutoThreshold()));
    connect(ui->pushButtonGenerate,SIGNAL(clicked()),SLOT(generateResult()));
}

WizardPagePBCDAutoThreshold::~WizardPagePBCDAutoThreshold()
{
    delete ui;
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (poDriver)
        poDriver->Delete(mergeImagePath.toUtf8().constData());
}

double WizardPagePBCDAutoThreshold::posT() const
{
    return ui->doubleSpinBoxPosT->value();
}

double WizardPagePBCDAutoThreshold::negT() const
{
    return ui->doubleSpinBoxNegT->value();
}

void WizardPagePBCDAutoThreshold::initializePage()
{
    isGenerated = false;
    updateParams();
    updateHistogram();
}

bool WizardPagePBCDAutoThreshold::validatePage()
{
    return isGenerated;
}

void WizardPagePBCDAutoThreshold::applyAutoThreshold()
{
    QString autoMethodName = ui->comboBoxAutoThresholdMethod->currentText();
    CDTAutoThresholdInterface *thresholdPlugin = NULL;
    foreach (CDTAutoThresholdInterface *plugin, autoThresholdPlugins) {
        if (plugin->methodName()==autoMethodName)
        {
            thresholdPlugin = plugin;
        }
    }
    if (thresholdPlugin==NULL)
    {
        QMessageBox::critical(this,tr("Error"),tr("No Auto Threshold Method Found!"));
        return;
    }

    if (numOfThresholds==1)
    {
        double t = thresholdPlugin->autoThreshold(histogramPositive)*(maxVal-minVal)/256.+minVal;
        ui->doubleSpinBoxPosT->setValue(t);
//        qDebug()<<minVal<<maxVal<<t;
//        qDebug()<<histogramPositive;
    }
    else if(numOfThresholds==2)
    {
        double tPos = thresholdPlugin->autoThreshold(histogramPositive)*(maxVal-0)/256.+0;
        double tNeg = thresholdPlugin->autoThreshold(histogramPositive)*(0-minVal)/256.+minVal;
        ui->doubleSpinBoxPosT->setValue(tPos);
        ui->doubleSpinBoxNegT->setValue(tNeg);
    }
}

void WizardPagePBCDAutoThreshold::generateResult()
{
    //Get temp file
    auto GetTempFile = [](QString suffix){
        return QDir::tempPath() + "/" + QUuid::createUuid().toString() + suffix;
    };

    //A Lambda Functor to Open GDALDataset
    auto OpenGDALDataset = [&](const QString& path)->GDALDataset*{
        GDALDataset* poDS = (GDALDataset*)GDALOpen(path.toUtf8().constData(),GA_ReadOnly);
        if (poDS==NULL)
            throw tr("Open File: %1 Failed!").arg(path);
        return poDS;
    };

    //A Lambda Functor to Create GeoTiff Image GDALDataset
    auto CreateTiffDataset = [&](const QString& path,int width,int height,int bandCount,GDALDataType type)->GDALDataset*{
        GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
        if (poDriver == NULL)
            throw tr("No GeoTiff Driver!");
        char** pszOptions = NULL;
        pszOptions = CSLSetNameValue(pszOptions,"COMPRESS","DEFLATE");
        pszOptions = CSLSetNameValue(pszOptions,"PREDICTOR","1");
        pszOptions = CSLSetNameValue(pszOptions,"ZLEVEL","9");
        GDALDataset *poDS = (GDALDataset *)
                (poDriver->Create(path.toUtf8().constData(),width,height,bandCount,type,pszOptions));
        if (poDS == NULL)
            throw tr("Create diff image: %1 failed!").arg(path);
        return poDS;
    };

    GDALDataset *poMergeDS = OpenGDALDataset(mergeImagePath);
    resultImagePath = GetTempFile(".tif");
    GDALDataset *poResultDS = CreateTiffDataset(
                resultImagePath,
                poMergeDS->GetRasterXSize(),
                poMergeDS->GetRasterYSize(),
                1,GDT_Byte);
    double geoTrans[6];
    poMergeDS->GetGeoTransform(geoTrans);
    poResultDS->SetGeoTransform(geoTrans);
    poResultDS->SetProjection(poMergeDS->GetProjectionRef());

    const double posT = ui->doubleSpinBoxPosT->value();
    const double negT = ui->doubleSpinBoxNegT->value();

    CDTPBCDGenerateResult *thread = new CDTPBCDGenerateResult(poMergeDS,poResultDS,numOfThresholds,posT,negT,this);
    this->setEnabled(false);
    connect(thread,SIGNAL(finished()),SLOT(onResultGenerated()));
    connect(thread,SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    connect(thread,SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(thread,SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    thread->start();
}

void WizardPagePBCDAutoThreshold::onHistogramGenerated()
{
    this->setEnabled(true);
    ui->labelProgress->clear();
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);
    CDTPBCDHistogramHelper *thread = qobject_cast<CDTPBCDHistogramHelper *>(sender());
    if (thread)
    {
        maxVal = thread->maxVal;
        minVal = thread->minVal;
        std::transform(thread->histogramNegetive.begin(),thread->histogramNegetive.end(),histogramNegetive.begin(),[=](GUIntBig s)->int{return static_cast<int>(s);});
        std::transform(thread->histogramPositive.begin(),thread->histogramPositive.end(),histogramPositive.begin(),[=](GUIntBig s)->int{return static_cast<int>(s);});
        thread->deleteLater();

        if (numOfThresholds==1)
        {
            ui->doubleSpinBoxPosT->setMinimum(minVal);
            ui->doubleSpinBoxPosT->setMaximum(maxVal);
        }
        else if (numOfThresholds==2)
        {
            ui->doubleSpinBoxPosT->setMinimum(0);
            ui->doubleSpinBoxPosT->setMaximum(maxVal);
            ui->doubleSpinBoxNegT->setMinimum(minVal);
            ui->doubleSpinBoxNegT->setMaximum(0);
        }

        //Generate default thresold
        if (ui->comboBoxAutoThresholdMethod->count()!=0)
            ui->pushButtonThreshold->click();
    }
}

void WizardPagePBCDAutoThreshold::onResultGenerated()
{
    this->setEnabled(true);
    CDTPBCDGenerateResult *thread = qobject_cast<CDTPBCDGenerateResult *>(sender());
    if (thread)
    {
        isGenerated = true;
        thread->deleteLater();
    }
}

void WizardPagePBCDAutoThreshold::updateParams()
{
    //Get the main QWizard
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

    //Fetch and setup the params
    mergeImagePath = wizard->property("FloatImage").toString();
    numOfThresholds = wizard->property("NumOfThresholds").toInt();
    ui->labelNeg->setEnabled(numOfThresholds!=1);
    ui->doubleSpinBoxNegT->setEnabled(numOfThresholds!=1);
}

void WizardPagePBCDAutoThreshold::updateHistogram()
{
    GDALDataset *poDS = (GDALDataset *)GDALOpen(mergeImagePath.toUtf8().constData(),GA_ReadOnly);
    if (poDS==NULL)
    {
        qDebug()<<"Null dataset!";
        return;
    }

    CDTPBCDHistogramHelper *thread = new CDTPBCDHistogramHelper(poDS,numOfThresholds,this);
    this->setEnabled(false);
    connect(thread,SIGNAL(finished()),SLOT(onHistogramGenerated()));
    connect(thread,SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    connect(thread,SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(thread,SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    thread->start();
}
