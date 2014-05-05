#ifndef DIALOGNEWPROJECT_H
#define DIALOGNEWPROJECT_H

#include <QDialog>

namespace Ui {
class DialogNewProject;
}

class DialogNewProject : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewProject(QWidget *parent = 0);
    ~DialogNewProject();

    QString projectName() const;
    QString projectPath() const;

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogNewProject *ui;
};

#endif // DIALOGNEWPROJECT_H
