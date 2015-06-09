#ifndef WIZARDPAGEPBCDDIFF_H
#define WIZARDPAGEPBCDDIFF_H

#include <QWizardPage>

namespace Ui {
class WizardPagePBCDDiff;
}

struct QUuid;
class QSqlQueryModel;

class WizardPagePBCDDiff : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardPagePBCDDiff(QUuid projectID,QWidget *parent = 0);
    ~WizardPagePBCDDiff();

private slots:
    void onT1ImageChanged(int row);
    void onT2ImageChanged(int row);

    void onAddBandPair();
    void onAutoBand();
    void onRemoveBands();
    void onSelectionChanged();
    void updatePushbuttonRemoveAll();
    void clearBandPairs();

    void updateGroupBoxMerge();
    void generate();

private:
    Ui::WizardPagePBCDDiff *ui;
    QUuid prjID;
    QSqlQueryModel *modelImage;
};

#endif // WIZARDPAGEPBCDDIFF_H
