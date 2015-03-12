#ifndef WIZARDVECTORCHANGEDETECTION_H
#define WIZARDVECTORCHANGEDETECTION_H

#include <QWizard>

namespace Ui {
class WizardVectorChangeDetection;
}

class WizardVectorChangeDetection : public QWizard
{
    Q_OBJECT

public:
    explicit WizardVectorChangeDetection(QWidget *parent = 0);
    ~WizardVectorChangeDetection();

private:
    Ui::WizardVectorChangeDetection *ui;
};

#endif // WIZARDVECTORCHANGEDETECTION_H
