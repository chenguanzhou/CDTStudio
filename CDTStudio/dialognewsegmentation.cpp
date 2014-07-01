#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtsegmentationinterface.h"

extern QList<CDTSegmentationInterface *> segmentationPlugins;

DialogNewSegmentation::DialogNewSegmentation(
        const QString &inputImage,
        CDTFileSystem* fileSys,
        QWidget *parent) :
    QDialog(parent),
    fileSystem(fileSys),
    ui(new Ui::DialogNewSegmentation),
    inputImagePath(inputImage)
{
    ui->setupUi(this);
    loadPlugins();

    markfileTempPath  = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".tif";
    shapefileTempPath = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".shp";

    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    ui->colorPicker->setStandardColors();
    this->adjustSize();
}

DialogNewSegmentation::~DialogNewSegmentation()
{
    delete ui;
}

QString DialogNewSegmentation::name() const
{
    return ui->lineEditName->text();
}

QString DialogNewSegmentation::markfilePath() const
{
    return markfileID;
}

QString DialogNewSegmentation::shapefilePath() const
{
    return shapefileID;
}

QColor DialogNewSegmentation::borderColor() const
{
    return ui->colorPicker->currentColor();
}

QString DialogNewSegmentation::method() const
{
    return ui->comboBox->currentText();
}

QVariantMap DialogNewSegmentation::params() const
{
    return segmentationParams;
}

void DialogNewSegmentation::on_comboBox_currentIndexChanged(int index)
{    
    ui->propertyWidget->setObject(segmentationPlugins[index]);
}

void DialogNewSegmentation::loadPlugins()
{
    foreach (CDTSegmentationInterface* plugin, segmentationPlugins) {
        ui->comboBox->addItem(plugin->segmentationMethod());
        plugin->setInputImagePath(inputImagePath);
    }
}

void DialogNewSegmentation::onFinished()
{
    disconnect(sender(),SIGNAL(finished()),this,SLOT(onFinished()));

    markfileID  = QUuid::createUuid().toString();
    shapefileID = QUuid::createUuid().toString();

    QString pathMarkZip = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".zip";
    CDTFileSystem::getRasterVSIZipFile(markfileTempPath,pathMarkZip,true);

    fileSystem->registerFile(shapefileID,shapefileTempPath,QString(),QString()
                             ,CDTFileSystem::getShapefileAffaliated(shapefileTempPath));
    fileSystem->registerFile(markfileID,pathMarkZip,"/vsizip/","/"+QFileInfo(markfileTempPath).fileName());

    ui->buttonBox->setStandardButtons(ui->buttonBox->standardButtons()|QDialogButtonBox::Ok);
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    this->adjustSize();    
    ui->frameTotal->setEnabled(true);
    ui->pushButtonStart->setEnabled(true);
}

void DialogNewSegmentation::on_pushButtonStart_clicked()
{
    ui->progressBar->show();
    ui->labelProgress->show();
    this->adjustSize();

    CDTSegmentationInterface *interface = segmentationPlugins[ui->comboBox->currentIndex()];
    interface->setMarkfilePath(markfileTempPath);
    interface->setShapefilePath(shapefileTempPath);

    connect(interface,SIGNAL(finished()),this,SLOT(onFinished()));
    interface->startSegmentation(ui->progressBar,ui->labelProgress);

    segmentationParams = interface->params();
    ui->frameTotal->setEnabled(false);
    ui->pushButtonStart->setEnabled(false);
}
