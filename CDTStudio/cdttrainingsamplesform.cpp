#include "cdttrainingsamplesform.h"
#include "ui_cdttrainingsamplesform.h"
#include "cdtmaptoolselecttrainingsamples.h"
#include "cdtimagelayer.h"

CDTTrainingSamplesForm::CDTTrainingSamplesForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDTTrainingSamplesForm)
{
    ui->setupUi(this);
    //    initForm();
}

CDTTrainingSamplesForm::~CDTTrainingSamplesForm()
{
    delete ui;
}

void CDTTrainingSamplesForm::clear()
{
    ui->listWidget->clear();

}

void CDTTrainingSamplesForm::updateCategoryInfo()
{
    clear();
    foreach (CategoryInformation info, imageLayer->categoryInformationList) {
        QPixmap pixmap(16,12);
        pixmap.fill(info.color);
        ui->listWidget->addItem(new QListWidgetItem(QIcon(pixmap),info.categoryName,ui->listWidget));
    }
}

void CDTTrainingSamplesForm::setImageLayer(CDTImageLayer *layer)
{
    clear();    
    imageLayer = layer;
    connect(imageLayer,SIGNAL(imageLayerChanged()),SLOT(updateCategoryInfo()));
    updateCategoryInfo();
}

//void CDTTrainingSamplesForm::initForm()
//{
//    QImage img(16,16,QImage::Format_RGB888);
//    img.fill(Qt::blue);

//    QListWidgetItem* item = new QListWidgetItem(QIcon(QPixmap::fromImage(img)),"water",ui->listWidget);
//    ui->listWidget->addItem(item);
//}
