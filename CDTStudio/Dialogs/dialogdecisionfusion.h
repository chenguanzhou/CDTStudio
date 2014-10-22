#ifndef DIALOGDECISIONFUSION_H
#define DIALOGDECISIONFUSION_H

#include <QDialog>

namespace Ui {
class DialogDecisionFusion;
}

class DialogDecisionFusion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDecisionFusion(QString segID, QWidget *parent = 0);
    ~DialogDecisionFusion();

private slots:
    void onSelectionChanged();
    void startDecisionFusion();

private:
    Ui::DialogDecisionFusion *ui;

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
};

#endif // DIALOGDECISIONFUSION_H
