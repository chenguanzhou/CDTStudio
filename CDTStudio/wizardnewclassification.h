#ifndef WIZARDNEWCLASSIFICATION_H
#define WIZARDNEWCLASSIFICATION_H

#include <QWizard>

namespace Ui {
class WizardNewClassification;
}
class QSqlQueryModel;
class QTreeWidgetItem;
class QStringListModel;

class WizardNewClassification : public QWizard
{
    Q_OBJECT

public:
    explicit WizardNewClassification(QUuid segmentationID,QWidget *parent = 0);
    ~WizardNewClassification();

    bool isValid()const;

private:
    void initClassifiers();
    void updateFeatures(QString segID);
    static QStringList attributeNames();
    bool validateCurrentPage();
    void startClassification();


private slots:
    void onSegmentationChanged(int index);
    void onClassifierChanged(int index);
    void onCurrentPageChanged(int pageID);
    void onButtonClicked(int buttonID);
    void updateSelectedFeature();
    void updateHistogram();

public:
    //Export
    QString name;
    QString method;
    QList<QVariant> label;//QList<int>
    QMap<QString,QVariant> categoryID_Index;//QMap<QString,int>
    QMap<QString,QVariant> params;

private:
    Ui::WizardNewClassification *ui;
    QSqlQueryModel *modelSample;
    QStringListModel *modelSelectedFeature;
    QList<QWidget*> paramWidgets;
    bool finished;
};

#endif // WIZARDNEWCLASSIFICATION_H
