#include "dialogabout.h"
#include "ui_dialogabout.h"

#include <QtCore>

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("%1 %2 %3")
                         .arg(tr("About"))
                         .arg(qApp->applicationName())
                         .arg(qApp->applicationVersion()));

    ui->labelAppName->setText(qApp->applicationName());
    ui->labelAppVersion->setText(qApp->applicationVersion());

    ui->tableWidget3rdParty->setColumnCount(3);
    ui->tableWidget3rdParty->setHorizontalHeaderLabels(QStringList()<<tr("Name")<<tr("Version")<<tr("Link"));
    QFile file("://3rdLicenses.txt");
    file.open(QFile::ReadOnly);
    QTextStream s(&file);
    s.setCodec("UTF-8");
    while (!s.atEnd()) {
        QStringList line = s.readLine().split("\t");
        qDebug()<<line;
        int rowID = ui->tableWidget3rdParty->rowCount();
        ui->tableWidget3rdParty->insertRow(rowID);
        for (int i=0;i<3;++i)
        {
            ui->tableWidget3rdParty->setItem(rowID,i,new QTableWidgetItem(line[i]));
        }
    }
    file.close();
    ui->tableWidget3rdParty->resizeColumnsToContents();


    connect(ui->commandLinkButtonOfficialPage,SIGNAL(clicked(bool)),SLOT(onOfficialPageLinkClicked()));
    connect(ui->tableWidget3rdParty,SIGNAL(clicked(QModelIndex)),SLOT(onTable3rdPartyItemClicked(QModelIndex)));
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::onOfficialPageLinkClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/chenguanzhou/CDTStudio"));
}

void DialogAbout::onTable3rdPartyItemClicked(QModelIndex index)
{
    if (index.column()==2)
        QDesktopServices::openUrl(QUrl(ui->tableWidget3rdParty->item(index.row(),index.column())->text()));
}
