#include "formmst.h"
#include "ui_formmst.h"
#include "mstsegmenter.h"
#include <QtCore>

FormMST::FormMST(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMST)
{
    ui->setupUi(this);    
    ui->progressBar->hide();
    ui->labelProgress->hide();
    this->adjustSize();
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
    ui->progressBar->show();
    ui->labelProgress->show();
    this->adjustSize();
    MSTSegmenter* mstSegmenter = new MSTSegmenter(
                interface->inputImagePath(),
                interface->markfilePath(),
                interface->shapefilePath(),
                ui->comboBoxMergeRule->currentIndex(),
                ui->doubleSpinBoxThreshold->value(),
                ui->spinBoxMinArea->value(),
                ui->checkBox->isChecked());

    connect(mstSegmenter, SIGNAL(finished()), this, SIGNAL(finished()));
    connect(mstSegmenter, SIGNAL(finished()), ui->labelProgress, SLOT(hide()));
    connect(mstSegmenter, SIGNAL(finished()), ui->progressBar, SLOT(hide()));
    connect(mstSegmenter, SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(mstSegmenter, SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    connect(mstSegmenter, SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    mstSegmenter->start();
    //    connect(mstSegmenter, SIGNAL(showWarningMessage(QString)),this,SLOT(onWarningMessage(QString)));
}
