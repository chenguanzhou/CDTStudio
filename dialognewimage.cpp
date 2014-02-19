#include "dialognewimage.h"
#include "ui_dialognewimage.h"
#include <QFileDialog>
DialogNewImage::DialogNewImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewImage)
{
    ui->setupUi(this);
}

DialogNewImage::~DialogNewImage()
{
    delete ui;
}

QString DialogNewImage::imageName() const
{
    return ui->lineEditName->text();
}

QString DialogNewImage::imagePath() const
{
    return ui->lineEditPath->text();
}

void DialogNewImage::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,tr("Open image"),QString(),"Images (*.png *.xpm *.jpg)");
    if(path.isEmpty())
        return;
    ui->lineEditPath->setText(path);
}
