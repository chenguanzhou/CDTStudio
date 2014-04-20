#include "dialognewclassification.h"
#include "ui_dialognewclassification.h"

DialogNewClassification::DialogNewClassification(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewClassification)
{
    ui->setupUi(this);
}

DialogNewClassification::~DialogNewClassification()
{
    delete ui;
}
