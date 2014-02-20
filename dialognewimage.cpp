#include "dialognewimage.h"
#include "ui_dialognewimage.h"
#include <QFileDialog>
#include <QFileInfo>

DialogNewImage::DialogNewImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewImage)
{
    ui->setupUi(this);
    setWindowTitle("Add image");
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
    QString path = QFileDialog::getOpenFileName(this,tr("Open image"),QString(),"Images (*.png *.xpm *.jpg *.img)");
    if(path.isEmpty())
        return;
    QFileInfo fileinfo(path);
    ui->lineEditPath->setText(path);
    ui->lineEditName->setText(fileinfo.fileName());
}
