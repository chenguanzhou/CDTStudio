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
    inputImagePath(inputImage),
    isFinished(false),
    isDBTested(false),
    ui(new Ui::DialogNewSegmentation)
{
    ui->setupUi(this);    

    markfileTempPath  = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".tif";
    shapefileTempPath = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".shp";

    updateButtonBoxStatus();
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    ui->colorPicker->setStandardColors();
    this->adjustSize();

    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),SLOT(setSegMethod(int)));
    connect(ui->pushButtonStart,SIGNAL(clicked()),SLOT(startSegmentation()));
    connect(ui->pushButtonDBInfo,SIGNAL(clicked()),SLOT(setDBConnectionInfo()));

    //should connect before load plugins
    loadPlugins();
}

DialogNewSegmentation::~DialogNewSegmentation()
{
    delete ui;
}

QString DialogNewSegmentation::name() const
{
    return ui->lineEditName->text();
}

QString DialogNewSegmentation::markfileID() const
{
    return mkID;
}

QString DialogNewSegmentation::shapefileID() const
{
    return shpID;
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

CDTDatabaseConnInfo DialogNewSegmentation::databaseConnInfo() const
{
    return dbConnInfo;
}

void DialogNewSegmentation::setSegMethod(int index)
{    
    ui->propertyWidget->setObject(segmentationPlugins[index]);
}

void DialogNewSegmentation::setDBConnectionInfo()
{
    DialogDBConnection dlg(dbConnInfo);
    if (dlg.exec()==QDialog::Accepted)
    {
        dbConnInfo = dlg.dbConnectInfo();
        isDBTested = true;
        updateButtonBoxStatus();
    }
}

void DialogNewSegmentation::loadPlugins()
{
    foreach (CDTSegmentationInterface* plugin, segmentationPlugins) {
        ui->comboBox->addItem(plugin->segmentationMethod());
        plugin->setInputImagePath(inputImagePath);
    }
}

void DialogNewSegmentation::startSegmentation()
{
    ui->progressBar->show();
    ui->labelProgress->show();
    this->adjustSize();

    CDTSegmentationInterface *interface = segmentationPlugins[ui->comboBox->currentIndex()];
    interface->setMarkfilePath(markfileTempPath);
    interface->setShapefilePath(shapefileTempPath);

    connect(interface,SIGNAL(finished()),this,SLOT(onSegFinished()));
    interface->startSegmentation(ui->progressBar,ui->labelProgress);

    segmentationParams = interface->params();
    ui->frameTotal->setEnabled(false);
    ui->pushButtonStart->setEnabled(false);
}

void DialogNewSegmentation::onSegFinished()
{
    disconnect(sender(),SIGNAL(finished()),this,SLOT(onSegFinished()));

    mkID  = QUuid::createUuid().toString();
    shpID = QUuid::createUuid().toString();

    fileSystem->registerFile(shpID,shapefileTempPath,QString(),QString(),
                             CDTFileSystem::getShapefileAffaliated(shapefileTempPath));
    fileSystem->registerFile(mkID,markfileTempPath,QString(),QString(),
                             CDTFileSystem::getRasterAffaliated(markfileTempPath));
    qDebug()<<CDTFileSystem::getRasterAffaliated(markfileTempPath);

    isFinished = true;
    updateButtonBoxStatus();
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    this->adjustSize();    
    ui->frameTotal->setEnabled(true);
    ui->pushButtonStart->setEnabled(true);
}

void DialogNewSegmentation::updateButtonBoxStatus()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isFinished&&isDBTested);
}
