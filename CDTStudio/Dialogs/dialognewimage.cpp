#include "dialognewimage.h"
#include "ui_dialognewimage.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtlayernamevalidator.h"

DialogNewImage::DialogNewImage(QUuid prjID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewImage)
{
    ui->setupUi(this);
    setWindowTitle("Add image");

    connect(ui->lineEditPath,SIGNAL(textChanged(QString)),SLOT(onPathChanged(QString)));
    ui->lineEditPath->setText(tr("new image"));
    ui->lineEditPath->setFilter(tr("Images (*.bmp *.png *.xpm *.jpg *.img *.tif)"));


    int index = CDTImageLayer::staticMetaObject.indexOfClassInfo("tableName");
    if (index != -1)
    {
        CDTLayerNameValidator *validator = new CDTLayerNameValidator
                (QSqlDatabase::database("category"),"name",CDTImageLayer::staticMetaObject.classInfo(index).value(),QString("projectid='%1'").arg(prjID));
        ui->lineEditName->setValidator(validator);
    }
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

bool DialogNewImage::useRelativePath() const
{
    return ui->checkBoxUseRelativePath->isChecked();
}

void DialogNewImage::onPathChanged(QString path)
{
    QFileInfo info(path);
    bool ret = info.isReadable()&&info.isFile();
    ui->lineEditName->setText(ret?info.fileName():QString::null);
    ui->lineEditName->setEnabled(ret);
}

//void DialogNewImage::loadHistoryPaths()
//{
//    QSettings setting("WHU","CDTStudio");
//    setting.beginGroup("Project");
//    QStringList imagedirs = setting.value("lastImageDirs").toStringList();
//    foreach (QString dir, imagedirs) {
//        ui->comboBox->addItem(dir);
//    }
//    setting.endGroup();
//}

//void DialogNewImage::saveHistoryPaths()
//{
//    QSettings setting("WHU","CDTStudio");
//    setting.beginGroup("Project");
//    QStringList dirs;
//    for(int i=0; i< ui->comboBox->count();++i)
//    {
//        dirs.push_back(ui->comboBox->itemText(i));
//    }
//    setting.setValue("lastImageDirs",dirs);
//    setting.endGroup();
//}
