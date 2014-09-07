#include "dialogpbcdbinary.h"
#include "ui_dialogpbcdbinary.h"

DialogPBCDBinary::DialogPBCDBinary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPBCDBinary)
{
    ui->setupUi(this);
}

DialogPBCDBinary::~DialogPBCDBinary()
{
    delete ui;
}
