#ifndef CDTCLASSIFIERASSESSMENTFORM_H
#define CDTCLASSIFIERASSESSMENTFORM_H

namespace Ui {
class CDTClassifierAssessmentForm;
}

class QSqlQueryModel;

class CDTClassificationInformation
{
public:
    //GeneralInformation
    QString     clsName;
    QString     classifierName;
    QVariantMap classifierParams;
    int         categoryCount;
    bool        isNormalized;
    QString     pcaParams;
    QStringList selectedFeatures;
    //ConfusionMat
    QStringList categories;
    QList<QPair<QString,QString> > confusionParams;//<result, validation sample>
};


class CDTClassifierAssessmentForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDTClassifierAssessmentForm(QWidget *parent = 0);
    ~CDTClassifierAssessmentForm();

    void setClassification(QString id);
private:
    void init();

private slots:
    void onComboBoxClassificationChanged(int index);
    void onComboBoxSampleChanged(int index);

private:
    void setInfo(const CDTClassificationInformation& info);
    void updateGeneralInfo(const CDTClassificationInformation& info);
    void updateConfusionMatrix(const CDTClassificationInformation& info);

    Ui::CDTClassifierAssessmentForm *ui;
    QSqlQueryModel *modelClassification;
    QSqlQueryModel *modelSample;
};

#endif // CDTCLASSIFIERASSESSMENTFORM_H
