#ifndef DIALOGPBCDBINARY_H
#define DIALOGPBCDBINARY_H

#include <QDialog>

struct QUuid;
class QSqlQueryModel;
class CDTTaskReply;

namespace Ui {
class DialogPBCDBinary;
}

class DialogPBCDBinary : public QDialog
{
    Q_OBJECT

public:
    friend class DialogPBCD;
    explicit DialogPBCDBinary(QUuid projectID,QWidget *parent = 0);
    ~DialogPBCDBinary();

    static CDTTaskReply *startBinaryPBCD(QUuid prjID);

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
    void updateDoubleThreshold();

    void onAutoThresholdToggled(bool toggled);
    void onManualThresholdToggled(bool toggled);

    void generateXML();

//    void onCompleted(QByteArray result);

private:
    Ui::DialogPBCDBinary *ui;

    CDTTaskReply *reply;
    QUuid prjID;
    QSqlQueryModel *modelImage;
    bool isDoubleThreshold;
};

#endif // DIALOGPBCDBINARY_H
