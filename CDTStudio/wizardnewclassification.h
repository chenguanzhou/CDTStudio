#ifndef WIZARDNEWCLASSIFICATION_H
#define WIZARDNEWCLASSIFICATION_H

#include <QWizard>

namespace Ui {
class WizardNewClassification;
}

class WizardNewClassification : public QWizard
{
    Q_OBJECT

public:
    explicit WizardNewClassification(QWidget *parent = 0);
    ~WizardNewClassification();

private:
    Ui::WizardNewClassification *ui;
};

#endif // WIZARDNEWCLASSIFICATION_H
