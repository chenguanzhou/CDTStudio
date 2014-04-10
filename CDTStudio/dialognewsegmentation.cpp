#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

extern QList<CDTSegmentationInterface *> segmentationPlugins;

DialogNewSegmentation::DialogNewSegmentation(const QString &inputImage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewSegmentation),
    gridLatoutPlugin(new QGridLayout()),
    inputImagePath(inputImage)
{
    ui->setupUi(this);
    ui->frame->setLayout(gridLatoutPlugin);
    gridLatoutPlugin->setMargin(0);
    loadPlugins();
    loadHistoryPaths();
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    this->adjustSize();
}

DialogNewSegmentation::~DialogNewSegmentation()
{
    saveHistoryPaths();
    delete ui;
}

QString DialogNewSegmentation::name() const
{
    return ui->lineEditName->text();
}

QString DialogNewSegmentation::markfilePath() const
{
    return ui->comboBoxMarkfile->currentText();
}

QString DialogNewSegmentation::shapefilePath() const
{
    return ui->comboBoxShapefile->currentText();
}

QString DialogNewSegmentation::method() const
{
    return ui->comboBox->currentText();
}

QMap<QString, QVariant> DialogNewSegmentation::params() const
{
    return segmentationParams;
}



void DialogNewSegmentation::saveHistoryPaths()
{
    QStringList markfilePathList;
    for (int i=0;i<ui->comboBoxMarkfile->count();++i)
        markfilePathList<<ui->comboBoxMarkfile->itemText(i);

    QStringList shapefilePathList;
    for (int i=0;i<ui->comboBoxShapefile->count();++i)
        shapefilePathList<<ui->comboBoxShapefile->itemText(i);

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    setting.setValue("MarkfilePathLists",markfilePathList);
    setting.setValue("ShapefilePathLists",shapefilePathList);
    setting.endGroup();
}

void DialogNewSegmentation::loadHistoryPaths()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QStringList markfilePathList = setting.value("MarkfilePathLists").toStringList();
    QStringList shapefilePathList = setting.value("ShapefilePathLists").toStringList();
    setting.endGroup();

    foreach (QString path, markfilePathList) {
        ui->comboBoxMarkfile->addItem(path);
    }

    foreach (QString path, shapefilePathList) {
        ui->comboBoxShapefile->addItem(path);
    }
}

void DialogNewSegmentation::on_comboBox_currentIndexChanged(int index)
{    
    if (gridLatoutPlugin->itemAtPosition(0,0) != NULL)
    {
        QWidget* lastWidget = gridLatoutPlugin->itemAtPosition(0,0)->widget();
        if (lastWidget!=NULL)
        {
            gridLatoutPlugin->removeWidget(lastWidget);
            delete lastWidget;
        }
    }
    gridLatoutPlugin->addWidget(segmentationPlugins[index]->paramsForm(),0,0);
    this->adjustSize();
}

void DialogNewSegmentation::on_pushButtonMarkfile_clicked()
{
    QString markfilePath = QFileDialog::getSaveFileName(this,tr("Markfile Path"),QFileInfo(inputImagePath).absolutePath(),"*.tif");
    if(markfilePath.isEmpty())
        return;

    for (int i=0;i<ui->comboBoxMarkfile->count();++i)
    {
        QString path = ui->comboBoxMarkfile->itemText(i);
        if (path == markfilePath)
        {
            ui->comboBoxMarkfile->removeItem(i);
            break;
        }
    }

    ui->comboBoxMarkfile->insertItem(0,markfilePath);
    ui->comboBoxMarkfile->setCurrentIndex(0);
}

void DialogNewSegmentation::on_pushButtonShapefile_clicked()
{
    QString shapefilePath = QFileDialog::getSaveFileName(this,tr("ShapefilePath Path"),QFileInfo(inputImagePath).absolutePath(),"*.shp");
    if(shapefilePath.isEmpty())
        return;

    for (int i=0;i<ui->comboBoxShapefile->count();++i)
    {
        QString path = ui->comboBoxShapefile->itemText(i);
        if (path == shapefilePath)
        {
            ui->comboBoxShapefile->removeItem(i);
            break;
        }
    }

    ui->comboBoxShapefile->insertItem(0,shapefilePath);
    ui->comboBoxShapefile->setCurrentIndex(0);
}

void DialogNewSegmentation::loadPlugins()
{
    foreach (CDTSegmentationInterface* plugin, segmentationPlugins) {
        ui->comboBox->addItem(plugin->segmentationMethod());
        plugin->setInputImagePath(inputImagePath);
        connect(plugin,SIGNAL(finished(QMap<QString,QVariant>)),this,SLOT(onFinished(QMap<QString,QVariant>)));
    }
}

void DialogNewSegmentation::on_comboBoxMarkfile_currentIndexChanged(const QString &arg1)
{
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex<segmentationPlugins.size())
        segmentationPlugins[currentIndex]->setMarkfilePath(arg1);
}

void DialogNewSegmentation::on_comboBoxShapefile_currentIndexChanged(const QString &arg1)
{
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex<segmentationPlugins.size())
        segmentationPlugins[currentIndex]->setShapefilePath(arg1);
}

void DialogNewSegmentation::onFinished()
{
    ui->buttonBox->setStandardButtons(ui->buttonBox->standardButtons()|QDialogButtonBox::Ok);
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    this->adjustSize();
    segmentationParams = segmentationPlugins[ui->comboBox->currentIndex()]
            ->params(gridLatoutPlugin->itemAtPosition(0,0)->widget());
    ui->frameTotal->setEnabled(true);
    ui->pushButtonStart->setEnabled(true);
}

void DialogNewSegmentation::on_pushButtonStart_clicked()
{
    ui->progressBar->show();
    ui->labelProgress->show();
    this->adjustSize();

    QThread *thread = segmentationPlugins[ui->comboBox->currentIndex()]
            ->thread(gridLatoutPlugin->itemAtPosition(0,0)->widget());
    connect(thread, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(thread, SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(thread, SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    connect(thread, SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    thread->start();
    ui->frameTotal->setEnabled(false);
    ui->pushButtonStart->setEnabled(false);
}
