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

    QString name() const;
    QString imageID_t1()const;
    QString imageID_t2()const;
    bool validatePage();    

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
    void setUnGenerated();

    void generationFinished();
    void showWarningMessage(QString msg);
    void generate();

private:
    Ui::WizardPagePBCDDiff *ui;
    QUuid prjID;
    QSqlQueryModel *modelImage;
    bool isGenerated;
};

#endif // WIZARDPAGEPBCDDIFF_H
