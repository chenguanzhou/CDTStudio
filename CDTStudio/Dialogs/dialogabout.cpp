#include "dialogabout.h"
#include "ui_dialogabout.h"

#include <QDesktopServices>

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

    connect(ui->commandLinkButtonOfficialPage,SIGNAL(clicked(bool)),SLOT(onOfficialPageLinkClicked()));
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::onOfficialPageLinkClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/chenguanzhou/CDTStudio"));
}
