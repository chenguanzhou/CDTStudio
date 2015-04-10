#ifndef WIZARDNEWCLASSIFICATION_H
#define WIZARDNEWCLASSIFICATION_H

#include <QWizard>
#include "log4qt/logger.h"
#include "cdtclassificationhelper.h"

namespace Ui {
class WizardNewClassification;
}
class QSqlQueryModel;
class QTreeWidgetItem;
class QStringListModel;

class WizardNewClassification : public QWizard
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit WizardNewClassification(QUuid segmentationID,QWidget *parent = 0);
    ~WizardNewClassification();

    bool    isValid()const;

private:
    void    initClassifiers();
    void    updateFeatures(QString segID);    
    bool    validateCurrentPage();
    void    startClassification();
//    void    generateAssessmentResult();
//    int     nextId() const;
//    void    initializePage(int id);

    QString segmentationID() const;
    QString imageID() const;

    static QStringList attributeNames();

private slots:
    void    onSegmentationChanged(int index);
    void    onClassifierChanged(int index);
    void    onCurrentPageChanged(int pageID);
    void    onButtonClicked(int buttonID);
    void    updateSelectedFeature();
    void    updateHistogram();

public:
    //Export
    QString                 name;
    QString                 method;
    QList<QVariant>         label;//QList<int>
    QMap<QString,QVariant>  categoryID_Index;//QMap<QString,int>
    QMap<int,QString>       samples;//objID_catID
    QMap<int,QString>       testSamples;//objID_catID
    QVariantMap             params;
    QString                 normalizeMethod;
    QString                 pcaParams;
    QStringList             featuresList;

private:
    Ui::WizardNewClassification *ui;
    QSqlQueryModel      *modelSample;
    QStringListModel    *modelSelectedFeature;
    QList<QWidget*>     paramWidgets;
    bool                finished;
};

#endif // WIZARDNEWCLASSIFICATION_H
