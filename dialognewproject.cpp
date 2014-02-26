#include "dialognewproject.h"
#include "ui_dialognewproject.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
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
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Project");
    QString filepath = setting.value("lastDir",".").toString();
    QString path = QFileDialog::getSaveFileName(this,tr("Create project file"),filepath,"*.cdtpro");
    if (path.isEmpty())
        return;
    QFileInfo fileinfo(path);
    ui->lineEditPath->setText(path);
    ui->lineEditName->setText(fileinfo.fileName());
    setting.setValue("lastDir",fileinfo.absolutePath());
    setting.endGroup();

    //set setting of recentFilePaths
    setting.beginReadArray("recentFilePaths");
    std::vector<std::string> paths;
    for (int i = 0; i < 5; ++i)
    {
        setting.setArrayIndex(i);
        QString p = setting.value("filePath").toString();
        paths.push_back(p.toStdString());
    }
    paths.resize(5);
    setting.endArray();

    if(std::find(paths.begin(),paths.end(),path.toStdString()) == paths.end())
    {
        setting.beginWriteArray("recentFilePaths");
        paths.insert(paths.begin(),path.toStdString());
        for(int i=0;i<paths.size();++i)
        {
            setting.setArrayIndex(i);
            setting.setValue("filePath",QString::fromStdString(paths[i]));
        }
        setting.endArray();
    }
}
