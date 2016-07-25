#include "dialognewproject.h"
#include "ui_dialognewproject.h"
#include "stable.h"

DialogNewProject::DialogNewProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewProject)
{
    ui->setupUi(this);
    ui->lineEditPath->setFilter("*.cdtpro");
    ui->lineEditPath->setAcceptMode(QFileDialog::AcceptSave);
    connect(ui->lineEditPath,SIGNAL(textChanged(QString)),SLOT(onButtonClicked()));
}

DialogNewProject::~DialogNewProject()
{
    delete ui;
}

QString DialogNewProject::projectName() const
{
    return ui->lineEditName->text();
}

QString DialogNewProject::projectPath() const
{
    return ui->lineEditPath->text();
}

void DialogNewProject::onButtonClicked()
{
    QString path = ui->lineEditPath->text();
    if (path.isEmpty())
        return;

    QFileInfo fileinfo(path);
    ui->lineEditName->setText(fileinfo.baseName());    
}
