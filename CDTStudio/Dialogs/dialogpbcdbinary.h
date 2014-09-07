#ifndef DIALOGPBCDBINARY_H
#define DIALOGPBCDBINARY_H

#include <QDialog>

namespace Ui {
class DialogPBCDBinary;
}

class DialogPBCDBinary : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPBCDBinary(QWidget *parent = 0);
    ~DialogPBCDBinary();

private:
    Ui::DialogPBCDBinary *ui;
};

#endif // DIALOGPBCDBINARY_H
