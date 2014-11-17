#include "dialoggeneratevalidationsample.h"
#include "ui_dialoggeneratevalidationsample.h"

DialogGenerateValidationSample::DialogGenerateValidationSample(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenerateValidationSample)
{
    ui->setupUi(this);
}

DialogGenerateValidationSample::~DialogGenerateValidationSample()
{
    delete ui;
}
