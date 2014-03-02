#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include <QDebug>
#include <MSTSegmenter.h>
#include <QFileDialog>
#include <QMessageBox>
#include "cdtpluginloader.h"

DialogNewSegmentation::DialogNewSegmentation(const QString &inputImage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewSegmentation),
    gridLatoutPlugin(new QGridLayout()),
    inputImagePath(inputImage)
{
    ui->setupUi(this);
    ui->frame->setLayout(gridLatoutPlugin);
    init();
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
    return ui->lineEditMarkfile->text();
}

QString DialogNewSegmentation::shapefilePath() const
{
    return ui->lineEditShapefile->text();
}

QString DialogNewSegmentation::method() const
{
    return ui->comboBox->currentText();
}

QMap<QString, QVariant> DialogNewSegmentation::params() const
{
    return QMap<QString, QVariant>();
}

void DialogNewSegmentation::init()
{    
    plugins = CDTPluginLoader<CDTSegmentationInterface>::getPlugins();
    foreach (CDTSegmentationInterface* plugin, plugins) {
        ui->comboBox->addItem(plugin->segmentationMethod());
        plugin->setInputImagePath(inputImagePath);
    }
}

void DialogNewSegmentation::on_comboBox_currentIndexChanged(int index)
{
    if (gridLatoutPlugin->itemAtPosition(0,0) != NULL)
    {
        QWidget* lastWidget = gridLatoutPlugin->itemAtPosition(0,0)->widget();
        gridLatoutPlugin->removeWidget(lastWidget);
        delete lastWidget;
    }
    gridLatoutPlugin->addWidget(plugins[index]->paramsForm(),0,0);
    this->adjustSize();
}

void DialogNewSegmentation::on_pushButtonMarkfile_clicked()
{
    QString markfilePath = QFileDialog::getSaveFileName(this,tr("Markfile Path"),QFileInfo(inputImagePath).absolutePath(),"*.tif");
    if(markfilePath.isEmpty())
        return;
    ui->lineEditMarkfile->setText(markfilePath);
}

void DialogNewSegmentation::on_pushButtonShapefile_clicked()
{
    QString shapefilePath = QFileDialog::getSaveFileName(this,tr("ShapefilePath Path"),QFileInfo(inputImagePath).absolutePath(),"*.shp");
    if(shapefilePath.isEmpty())
        return;
    ui->lineEditShapefile->setText(shapefilePath);
}

void DialogNewSegmentation::on_lineEditMarkfile_textChanged(const QString &arg1)
{
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex<plugins.size())
        plugins[currentIndex]->setMarkfilePath(arg1);
}

void DialogNewSegmentation::on_lineEditShapefile_textChanged(const QString &arg1)
{
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex<plugins.size())
        plugins[currentIndex]->setShapefilePath(arg1);
}
