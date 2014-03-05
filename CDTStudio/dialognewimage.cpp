#include "dialognewimage.h"
#include "ui_dialognewimage.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
DialogNewImage::DialogNewImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewImage)
{
    ui->setupUi(this);
    setWindowTitle("Add image");
    loadHistoryPaths();
}

DialogNewImage::~DialogNewImage()
{
    saveHistoryPaths();
    delete ui;
}

QString DialogNewImage::imageName() const
{
    return ui->lineEditName->text();
}

QString DialogNewImage::imagePath() const
{
    return ui->comboBox->itemText(ui->comboBox->currentIndex());
}

void DialogNewImage::on_pushButton_clicked()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QStringList imagedirs = setting.value("lastImageDirs").toStringList();
    QString dir;
    if(imagedirs.isEmpty())
    {
        dir = QFileDialog::getOpenFileName(this,tr("Open image"),".","Images (*.png *.xpm *.jpg *.img *.tif)");
    }
    else
    {
        dir = QFileDialog::getOpenFileName(this,tr("Open image"),imagedirs[0],"Images (*.png *.xpm *.jpg *.img *.tif)");
    }
    if(dir.isEmpty())
        return;
    for(int i=0;i< ui->comboBox->count();++i)
    {
        QString tempdir = ui->comboBox->itemText(i);
        if(dir == tempdir)
        {
            ui->comboBox->removeItem(i);
            break;
        }
    }
    ui->comboBox->insertItem(0,dir);
    ui->comboBox->setCurrentIndex(0);
    setting.endGroup();

}



void DialogNewImage::loadHistoryPaths()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QStringList imagedirs = setting.value("lastImageDirs").toStringList();
    foreach (QString dir, imagedirs) {
        ui->comboBox->addItem(dir);
    }
    setting.endGroup();
}

void DialogNewImage::saveHistoryPaths()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QStringList dirs;
    for(int i=0; i< ui->comboBox->count();++i)
    {
        dirs.push_back(ui->comboBox->itemText(i));
    }
    setting.setValue("lastImageDirs",dirs);
    setting.endGroup();
}

void DialogNewImage::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QFileInfo fileinfo(arg1);
    ui->lineEditName->setText(fileinfo.fileName());
}
