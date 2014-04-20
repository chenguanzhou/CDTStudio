#ifndef DIALOGNEWCLASSIFICATION_H
#define DIALOGNEWCLASSIFICATION_H

#include <QDialog>

namespace Ui {
class DialogNewClassification;
}

class DialogNewClassification : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewClassification(QWidget *parent = 0);
    ~DialogNewClassification();

private:
    Ui::DialogNewClassification *ui;
};

#endif // DIALOGNEWCLASSIFICATION_H
