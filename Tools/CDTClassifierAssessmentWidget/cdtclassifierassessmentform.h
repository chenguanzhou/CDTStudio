#ifndef CDTCLASSIFIERASSESSMENTFORM_H
#define CDTCLASSIFIERASSESSMENTFORM_H

#include <QtCore>
#include <QWidget>
#include "cdtclassifierassessmentwidget_global.h"

namespace Ui {
class CDTClassifierAssessmentForm;
}


class CDTClassificationInformation
{
public:
    //GeneralInformation
    QString     clsName;
    QString     classifierName;
    QVariantMap classifierParams;
    int         categoryCount;
    bool        isNormalized;
    int         pcaParams;
    QStringList selectedFeatures;
    //ConfusionMat
    QStringList categories;
    QList<QPair<QString,QString> > confusionParams;//<result,test sample>
};


class CDTCLASSIFIERASSESSMENTWIDGETSHARED_EXPORT CDTClassifierAssessmentForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDTClassifierAssessmentForm(QWidget *parent = 0);
    ~CDTClassifierAssessmentForm();

public slots:
    void setInfo(const CDTClassificationInformation& info);

private:
    void updateGeneralInfo(const CDTClassificationInformation& info);
    void updateConfusionMatrix(const CDTClassificationInformation& info);
private:
    Ui::CDTClassifierAssessmentForm *ui;
};

#endif // CDTCLASSIFIERASSESSMENTFORM_H
