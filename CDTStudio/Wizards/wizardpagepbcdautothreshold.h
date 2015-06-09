#ifndef WIZARDPAGEPBCDAUTOTHRESHOLD_H
#define WIZARDPAGEPBCDAUTOTHRESHOLD_H

#include <QWizardPage>

namespace Ui {
class WizardPagePBCDAutoThreshold;
}

class WizardPagePBCDAutoThreshold : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardPagePBCDAutoThreshold(QWidget *parent = 0);
    ~WizardPagePBCDAutoThreshold();

private:
    Ui::WizardPagePBCDAutoThreshold *ui;
};

#endif // WIZARDPAGEPBCDAUTOTHRESHOLD_H
