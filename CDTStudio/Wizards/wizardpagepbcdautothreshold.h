#ifndef WIZARDPAGEPBCDAUTOTHRESHOLD_H
#define WIZARDPAGEPBCDAUTOTHRESHOLD_H

#include <QWizardPage>
#include <QVector>

namespace Ui {
class WizardPagePBCDAutoThreshold;
}

class WizardPagePBCDAutoThreshold : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardPagePBCDAutoThreshold(QWidget *parent = 0);
    ~WizardPagePBCDAutoThreshold();

    void initializePage();
    bool validatePage();

private slots:
    void applyAutoThreshold();
    void generateResult();

    void onHistogramGenerated();
    void onResultGenerated();

private:
    void updateParams();
    void updateHistogram();

private:
    Ui::WizardPagePBCDAutoThreshold *ui;
    bool isGenerated;

    QString mergeImagePath;
    QString resultImagePath;

    int numOfThresholds;
    double maxVal;
    double minVal;
    QVector<int> histogramPositive;
    QVector<int> histogramNegetive;
};

#endif // WIZARDPAGEPBCDAUTOTHRESHOLD_H
