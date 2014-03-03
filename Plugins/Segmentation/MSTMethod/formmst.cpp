#include "formmst.h"
#include "ui_formmst.h"
#include "mstsegmenter.h"
#include <QtCore>

FormMST::FormMST(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMST)
{
    ui->setupUi(this);    
}

FormMST::~FormMST()
{
    delete ui;
}

void FormMST::setInterface(MSTMethodInterface *interface)
{
    this->interface=interface;
}

void FormMST::on_pushButtonStart_clicked()
{    
    qDebug()<<interface->inputImagePath();
    MSTSegmenter* mstSegmenter = new MSTSegmenter(
                interface->inputImagePath(),
                interface->markfilePath(),
                interface->shapefilePath(),
                ui->comboBoxMergeRule->currentIndex(),
                ui->doubleSpinBoxThreshold->value(),
                ui->spinBoxMinArea->value(),
                ui->pushButtonShield->isChecked());


//    this->setEnabled(false);    
    connect(mstSegmenter, SIGNAL(finished()), this, SIGNAL(finished()));
    connect(mstSegmenter, SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(mstSegmenter, SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    connect(mstSegmenter, SIGNAL(currentProgressChanged(QString)),this,SLOT(setGroupboxText(QString)));
    mstSegmenter->start();
    //    connect(mstSegmenter, SIGNAL(showWarningMessage(QString)),this,SLOT(onWarningMessage(QString)));
}

void FormMST::setGroupboxText(QString text)
{
    ui->groupBox->setTitle(text);
}
