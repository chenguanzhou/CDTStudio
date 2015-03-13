#ifndef WIZARDVECTORCHANGEDETECTION_H
#define WIZARDVECTORCHANGEDETECTION_H

#include <QWizard>
#include "log4qt/logger.h"

namespace Ui {
class WizardVectorChangeDetection;
}
struct QUuid;
class QComboBox;
class QSqlQueryModel;

class WizardVectorChangeDetection : public QWizard
{
/********************************************/
/*                  Wizard                  */
/********************************************/
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit WizardVectorChangeDetection(QUuid projectID,QWidget *parent = 0);
    ~WizardVectorChangeDetection();

private:
    bool validateCurrentPage();
private:
    Ui::WizardVectorChangeDetection *ui;
    QUuid prjID;
    bool isValid_Page1;

/********************************************/
/*                  Page 1                  */
/********************************************/
private:
    void initPage1();
    void updateCombobox(QComboBox *sender,QString IDFieldName,QSqlQueryModel *currentModel,QString tableName);
private slots:
    void updateT1SegLayer();
    void updateT1ClsLayer();
    void updateT2SegLayer();
    void updateT2ClsLayer();
    void onButtonShpT1Clicked();
    void onButtonShpT2Clicked();
    void updateReport_Page1();
    void showErrorText_Page1(QString msg);
    void showCorrectText_Page1(QString name, QString imageid_t1, QString segid_t1, QString clsid_t1, QString imageid_t2, QString segid_t2, QString clsid_t2, QString shapefile_t1, QString shapefile_t2, bool isUseLayer_t1, bool isUseLayer_t2);
private:
    QSqlQueryModel *model_ImageLayer;
    QSqlQueryModel *modelT1_SegLayer;
    QSqlQueryModel *modelT1_ClsLayer;
    QSqlQueryModel *modelT2_SegLayer;
    QSqlQueryModel *modelT2_ClsLayer;
};

#endif // WIZARDVECTORCHANGEDETECTION_H
