#ifndef FORMPARAMS_H
#define FORMPARAMS_H

#include <QWidget>

namespace Ui {
class FormParams;
}

class FormParams : public QWidget
{
    Q_OBJECT

public:
    explicit FormParams(QWidget *parent = 0);
    ~FormParams();

private:
    Ui::FormParams *ui;
};

#endif // FORMPARAMS_H
