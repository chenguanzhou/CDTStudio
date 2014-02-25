#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include <QDebug>
#include <MSTSegmenter.h>
#include <QFileDialog>
#include <QMessageBox>

DialogNewSegmentation::DialogNewSegmentation(const QString &inputImage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewSegmentation),
    gridLatoutParams(new QGridLayout()),
    inputImagePath(inputImage)
{
    ui->setupUi(this);
    initSegmentationMethod();
    ui->gridLayoutGroup->addLayout(gridLatoutParams,1,0);
    ui->labelProgressMessage->hide();
    ui->progressBar->hide();
    ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    connect(ui->checkBoxShapefilePath,SIGNAL(stateChanged(int)),this,SLOT(onLineEditChanged(int)));
    connect(ui->checkBoxMarkfilePath,SIGNAL(stateChanged(int)),this,SLOT(onLineEditChanged(int)));
    connect(ui->checkBoxSegmentName,SIGNAL(stateChanged(int)),this,SLOT(onLineEditChanged(int)));
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
    QMap<QString, QVariant> params;
    for (int i=0;i<gridLatoutParams->rowCount();++i)
    {
        QLabel *label = (QLabel *)(gridLatoutParams->itemAtPosition(i,0)->widget());
        QLineEdit* edit = (QLineEdit *)(gridLatoutParams->itemAtPosition(i,1)->widget());
        params[label->text()] = edit->text();
    }
    return params;
}

void DialogNewSegmentation::on_comboBox_currentIndexChanged(const QString &arg1)
{
    while(gridLatoutParams->count() > 0)
    {
        QWidget* widget = gridLatoutParams->itemAt(0)->widget();
        gridLatoutParams->removeWidget(widget);
        delete widget;
    }

    QStringList paramNames = segmentationParams[arg1].toStringList();
    for (int i=0;i<paramNames.size();++i)
    {
        QString paramName = paramNames[i];
        QLabel* label = new QLabel(paramName,this);
        QLineEdit* edit = new QLineEdit(this);
        edit->setObjectName("lineEditMST"+paramName);
        gridLatoutParams->addWidget(label,i,0);
        gridLatoutParams->addWidget(edit,i,1);
    }
    this->adjustSize();
}

void DialogNewSegmentation::initSegmentationMethod()
{
    segmentationParams.clear();
    segmentationParams[tr("mst")]  = QStringList()<<tr("Threshold")<<tr("Minimal Area");
    //    segmentationParams[tr("slic")] = QStringList()<<tr("Region size")<<tr("Regularizer");
    ui->comboBox->addItem(tr("mst"));
    //    ui->comboBox->addItem(tr("slic"));
    ui->comboBox->setCurrentIndex(0);
}

void DialogNewSegmentation::on_pushButtonStart_clicked()
{
    QLineEdit* lineEditThreshold = this->findChild<QLineEdit*>("lineEditMST"+tr("Threshold"));
    QLineEdit* lineEditMinArea = this->findChild<QLineEdit*>("lineEditMST"+tr("Minimal Area"));
    if (lineEditThreshold == NULL || lineEditMinArea == NULL)
        return;
    CDT::MSTSegmenter* mstSegmenter = new CDT::MSTSegmenter(
                inputImagePath,
                ui->lineEditMarkfile->text(),
                ui->lineEditShapefile->text(),
                0,
                lineEditThreshold->text().toDouble(),
                lineEditMinArea->text().toInt(),
                false);
    ui->labelProgressMessage->show();
    ui->progressBar->show();
    this->setEnabled(false);
    connect(mstSegmenter, SIGNAL(finished()), this, SLOT(onSegmentationFinished()));
    connect(mstSegmenter, SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(mstSegmenter, SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    connect(mstSegmenter, SIGNAL(currentProgressChanged(QString)),ui->labelProgressMessage,SLOT(setText(QString)));
    mstSegmenter->start();
    connect(mstSegmenter, SIGNAL(showWarningMessage(QString)),this,SLOT(onWarningMessage(QString)));
}


void DialogNewSegmentation::on_pushButtonBrowseMarkfilePath_clicked()
{
    QString markfilePath = QFileDialog::getSaveFileName(this,tr("Markfile Path"),QFileInfo(inputImagePath).absolutePath(),"*.tif");
    if(markfilePath.isEmpty())
        return;
    ui->lineEditMarkfile->setText(markfilePath);
}

void DialogNewSegmentation::on_pushButtonBrowseshapefilePath_clicked()
{
    QString shapefilePath = QFileDialog::getSaveFileName(this,tr("ShapefilePath Path"),QFileInfo(inputImagePath).absolutePath(),"*.shp");
    if(shapefilePath.isEmpty())
        return;
    ui->lineEditShapefile->setText(shapefilePath);
}

void DialogNewSegmentation::onLineEditChanged(int)
{

    ui->pushButtonStart->setEnabled(
                ui->checkBoxMarkfilePath->isChecked()&&
                ui->checkBoxShapefilePath->isChecked()&&
                ui->checkBoxSegmentName->isChecked()
                );
}

void DialogNewSegmentation::onSegmentationFinished()
{
    QMessageBox::information(this,tr("completed!"),tr("New segmentation ")+ui->lineEditName->text()+tr(" generated!"));
    QDialogButtonBox::StandardButtons buttons= ui->buttonBox->standardButtons();
    ui->buttonBox->setStandardButtons(buttons|QDialogButtonBox::Ok);
    this->setEnabled(true);
}

void DialogNewSegmentation::onWarningMessage(QString msg)
{

}

void DialogNewSegmentation::on_lineEditName_textChanged(const QString &arg1)
{
    ui->checkBoxSegmentName->setChecked(!arg1.isEmpty());
}

void DialogNewSegmentation::on_lineEditMarkfile_textChanged(const QString &arg1)
{
    if(QFileInfo(arg1).completeSuffix()==QString("tif"))
    {
        ui->checkBoxMarkfilePath->setChecked(QFileInfo(arg1).isWritable());
    }
    else
        ui->checkBoxMarkfilePath->setChecked(false);
}

void DialogNewSegmentation::on_lineEditShapefile_textChanged(const QString &arg1)
{
    if(QFileInfo(arg1).completeSuffix()==QString("shp"))
    {
        ui->checkBoxShapefilePath->setChecked(QFileInfo(arg1).isWritable());
    }
    else
        ui->checkBoxShapefilePath->setChecked(false);
}
