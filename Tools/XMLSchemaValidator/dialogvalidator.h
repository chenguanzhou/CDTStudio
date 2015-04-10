#ifndef DIALOGVALIDATOR_H
#define DIALOGVALIDATOR_H

#include <QDialog>

namespace Ui {
class DialogValidator;
}
class QAbstractButton;

class DialogValidator : public QDialog
{
    Q_OBJECT

public:
    explicit DialogValidator(QWidget *parent = 0);
    ~DialogValidator();

private slots:
    void on_pushButton_clicked();

private:
    void moveCursor(int line, int column);
    Ui::DialogValidator *ui;
};

#endif // DIALOGVALIDATOR_H
