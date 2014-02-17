#include "dialognewproject.h"
#include "ui_dialognewproject.h"
#include <QFileDialog>

DialogNewProject::DialogNewProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewProject)
{
    ui->setupUi(this);
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

void DialogNewProject::on_pushButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this,tr("Create project file"),QString(),"*.cdtpro");
    if (path.isEmpty())
        return;
    ui->lineEditPath->setText(path);
}
