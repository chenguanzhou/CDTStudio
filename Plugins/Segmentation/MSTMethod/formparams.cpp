#include "formparams.h"
#include "ui_formparams.h"

FormParams::FormParams(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormParams)
{
    ui->setupUi(this);
}

FormParams::~FormParams()
{
    delete ui;
}
