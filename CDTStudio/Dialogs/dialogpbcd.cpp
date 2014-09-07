#include "dialogpbcd.h"
#include "ui_dialogpbcd.h"
#include "stable.h"

#include "dialogpbcdbinary.h"

DialogPBCD::DialogPBCD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPBCD)
{
    ui->setupUi(this);
    connect(ui->toolButtonBinary,SIGNAL(clicked()),SLOT(onStartBinaryCD()));
    connect(ui->toolButtonFromTo,SIGNAL(clicked()),SLOT(onStartFromToCD()));
}

DialogPBCD::~DialogPBCD()
{
    delete ui;
}

void DialogPBCD::openPBCDDialog(QUuid projectID)
{
    if (projectID.isNull())
    {
        QMessageBox::critical(NULL,tr("Warning"),tr("Project ID is null!"));
        return;
    }

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select * from imagelayer where projectID = ?");
    query.addBindValue(projectID.toString());
    query.exec();

    QStringList imageLayerIDList;
    while(query.next())
    {
        imageLayerIDList<<query.value(0).toString();
    }

    if (imageLayerIDList.count()<2)
    {
        QMessageBox::critical(NULL,tr("Warning"),tr("The count of images in the current project is less than 2!"));
        return;
    }

    DialogPBCD dlg;
    dlg.prjID = projectID;
    dlg.imageLayerIDList = imageLayerIDList;
    dlg.exec();
}

void DialogPBCD::onStartBinaryCD()
{
    DialogPBCDBinary dlg;
    dlg.exec();
}

void DialogPBCD::onStartFromToCD()
{

}
