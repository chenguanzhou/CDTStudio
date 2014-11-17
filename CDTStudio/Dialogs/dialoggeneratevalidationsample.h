#ifndef DIALOGGENERATEVALIDATIONSAMPLE_H
#define DIALOGGENERATEVALIDATIONSAMPLE_H

#include <QDialog>

namespace Ui {
class DialogGenerateValidationSample;
}

class DialogGenerateValidationSample : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGenerateValidationSample(QWidget *parent = 0);
    ~DialogGenerateValidationSample();

private:
    Ui::DialogGenerateValidationSample *ui;
};

#endif // DIALOGGENERATEVALIDATIONSAMPLE_H
