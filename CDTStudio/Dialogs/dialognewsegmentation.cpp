#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtsegmentationinterface.h"
#include "cdtsegmentationhelper.h"
#include "cdtsegmentationlayer.h"
#include "cdtlayernamevalidator.h"

extern QList<CDTSegmentationInterface *> segmentationPlugins;

DialogNewSegmentation::DialogNewSegmentation(
        QUuid imageID,
        const QString &inputImage,
        CDTFileSystem* fileSys,
        QWidget *parent) :
    QDialog(parent),
    fileSystem(fileSys),
    inputImagePath(inputImage),
    isFinished(false),
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
    showPlugins();

    int index = CDTSegmentationLayer::staticMetaObject.indexOfClassInfo("tableName");
    if (index != -1)
    {
        CDTLayerNameValidator *validator = new CDTLayerNameValidator
                (QSqlDatabase::database("category"),"name",CDTSegmentationLayer::staticMetaObject.classInfo(index).value(),QString("imageid='%1'").arg(imageID));
        ui->lineEditName->setValidator(validator);
    }
    ui->lineEditName->setText(tr("Untitled"));
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

void DialogNewSegmentation::setSegMethod(int index)
{    
    ui->propertyWidget->setObject(segmentationPlugins[index]);
}

void DialogNewSegmentation::showPlugins()
{
    foreach (CDTSegmentationInterface* plugin, segmentationPlugins) {
        ui->comboBox->addItem(plugin->segmentationMethod());
        plugin->setInputImagePath(inputImagePath);
    }
}

void DialogNewSegmentation::startSegmentation()
{
    CDTSegmentationInterface *plugin = segmentationPlugins[ui->comboBox->currentIndex()];
    CDTSegmentationHelper *thread = new CDTSegmentationHelper(plugin,this);
    thread->setMarkfilePath(markfileTempPath);
    thread->setShapefilePath(shapefileTempPath);
    if (thread == NULL || !thread->isValid())
    {
        logger()->error("Start Segmentation Failed!");
        return;
    }

    connect(thread,SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    connect(thread,SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(thread,SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));

    connect(thread,SIGNAL(finished()),this,SLOT(onSegFinished()));

    ui->progressBar->show();
    ui->labelProgress->show();
    this->adjustSize();

    thread->start();

    segmentationParams = plugin->params();
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
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isFinished/*&&isDBTested*/);
}
