#ifndef DIALOGDECISIONFUSION_H
#define DIALOGDECISIONFUSION_H

#include <QDialog>

namespace Ui {
class DialogDecisionFusion;
}

class DialogDecisionFusion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDecisionFusion(QString segID, QWidget *parent = 0);
    ~DialogDecisionFusion();

private slots:
    void onSelectionChanged();

private:
    Ui::DialogDecisionFusion *ui;
};

#endif // DIALOGDECISIONFUSION_H
