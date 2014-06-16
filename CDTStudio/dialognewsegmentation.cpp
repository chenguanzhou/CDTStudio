#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

extern QList<CDTSegmentationInterface *> segmentationPlugins;

DialogNewSegmentation::DialogNewSegmentation(const QString &inputImage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewSegmentation),
    inputImagePath(inputImage)
{
    ui->setupUi(this);
    loadPlugins();
    loadHistoryPaths();
    ui->labelProgress->hide();
    ui->progressBar->hide();
    ui->frameTotal->adjustSize();
    ui->colorPicker->setStandardColors();
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
    ui->propertyWidget->setObject(segmentationPlugins[index]);
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
//        connect(plugin,SIGNAL(finished(QMap<QString,QVariant>)),this,SLOT(onFinished(QMap<QString,QVariant>)));
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
    disconnect(sender(),SIGNAL(finished()),this,SLOT(onFinished()));

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
    interface->setMarkfilePath(ui->comboBoxMarkfile->currentText());
    interface->setShapefilePath(ui->comboBoxShapefile->currentText());

    connect(interface,SIGNAL(finished()),this,SLOT(onFinished()));
    interface->startSegmentation(ui->progressBar,ui->labelProgress);

    segmentationParams = interface->params();
    ui->frameTotal->setEnabled(false);
    ui->pushButtonStart->setEnabled(false);
}
