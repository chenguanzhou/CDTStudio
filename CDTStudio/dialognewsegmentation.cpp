#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>


QList<CDTSegmentationInterface *> DialogNewSegmentation::plugins;

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
    return QMap<QString, QVariant>();
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
    gridLatoutPlugin->addWidget(plugins[index]->paramsForm(),0,0);
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
    if (plugins.size()==0)
        plugins = CDTPluginLoader<CDTSegmentationInterface>::getPlugins();
    foreach (CDTSegmentationInterface* plugin, plugins) {
        ui->comboBox->addItem(plugin->segmentationMethod());
        plugin->setInputImagePath(inputImagePath);
        connect(plugin,SIGNAL(finished()),this,SLOT(onFinished()));
    }
}

void DialogNewSegmentation::on_comboBoxMarkfile_currentIndexChanged(const QString &arg1)
{
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex<plugins.size())
        plugins[currentIndex]->setMarkfilePath(arg1);
}

void DialogNewSegmentation::on_comboBoxShapefile_currentIndexChanged(const QString &arg1)
{
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex<plugins.size())
        plugins[currentIndex]->setShapefilePath(arg1);
}

void DialogNewSegmentation::onFinished()
{
    ui->buttonBox->setStandardButtons(ui->buttonBox->standardButtons()|QDialogButtonBox::Ok);
}
