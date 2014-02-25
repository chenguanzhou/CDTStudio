#include "dialognewsegmentation.h"
#include "ui_dialognewsegmentation.h"
#include <QDebug>
#include <CDTBaseThread.h>

DialogNewSegmentation::DialogNewSegmentation(const QString &inputImage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewSegmentation),
    gridLatoutParams(new QGridLayout()),
    inputImagePath(inputImage)
{
    ui->setupUi(this);
    initSegmentationMethod();
    ui->gridLayoutGroup->addLayout(gridLatoutParams,1,0);
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
    qDebug()<<gridLatoutParams->rowCount();
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
        gridLatoutParams->addWidget(label,i,0);
        gridLatoutParams->addWidget(edit,i,1);
    }
    this->adjustSize();
}

void DialogNewSegmentation::initSegmentationMethod()
{
    segmentationParams.clear();
    segmentationParams[tr("mst")]  = QStringList()<<tr("Threshold")<<tr("Minimal area");
    segmentationParams[tr("slic")] = QStringList()<<tr("Region size")<<tr("Regularizer");
    ui->comboBox->addItem(tr("mst"));
    ui->comboBox->addItem(tr("slic"));
    ui->comboBox->setCurrentIndex(0);
}

void DialogNewSegmentation::on_pushButtonStart_clicked()
{

}

