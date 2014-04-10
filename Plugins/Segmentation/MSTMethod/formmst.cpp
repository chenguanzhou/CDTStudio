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

void FormMST::setInterface(CDTSegmentationInterface *interface)
{
    this->interface=interface;
}

QThread *FormMST::thread()
{
    MSTSegmenter* mstSegmenter = new MSTSegmenter(
                interface->inputImagePath(),
                interface->markfilePath(),
                interface->shapefilePath(),
                ui->comboBoxMergeRule->currentIndex(),
                ui->doubleSpinBoxThreshold->value(),
                ui->spinBoxMinArea->value(),
                ui->checkBox->isChecked());
    return mstSegmenter;
}

QMap<QString, QVariant> FormMST::params() const
{
    QMap<QString,QVariant> params;
    params.insert(tr("Merge Rule"),ui->comboBoxMergeRule->currentText());
    params.insert(tr("Threshold"),ui->doubleSpinBoxThreshold->value());
    params.insert(tr("Minimal Area"),ui->spinBoxMinArea->value());
    params.insert(tr("Shield No-data Value"),ui->checkBox->isChecked());
    return params;
}
