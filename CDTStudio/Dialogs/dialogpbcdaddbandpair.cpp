#include "dialogpbcdaddbandpair.h"
#include "ui_dialogpbcdaddbandpair.h"

#include "stable.h"

QString DialogPBCDAddBandPair::getNewPair(QString path1,
                                          QString path2)
{    
    DialogPBCDAddBandPair dlg(path1,path2);
    if (dlg.exec()==QDialog::Accepted)
        return dlg.ui->listWidgetT1->currentItem()->text() + "->" +
                dlg.ui->listWidgetT2->currentItem()->text();
    else
        return QString::null;
}

DialogPBCDAddBandPair::DialogPBCDAddBandPair(QString path1,
                                             QString path2,                                             
                                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPBCDAddBandPair)
{
    ui->setupUi(this);
    ui->labelT1->setText(path1);
    ui->labelT2->setText(path2);
    setListWidget(QgsRasterLayer(path1).bandCount(),ui->listWidgetT1);
    setListWidget(QgsRasterLayer(path2).bandCount(),ui->listWidgetT2);
}

DialogPBCDAddBandPair::~DialogPBCDAddBandPair()
{
    delete ui;
}

void DialogPBCDAddBandPair::setListWidget(int bandCount,QListWidget* widget)
{
    widget->clear();
    for(int i=0;i<bandCount;++i)
    {
        widget->addItem(QString("band%1").arg(i+1));
    }
    widget->addItem("ave");
}
