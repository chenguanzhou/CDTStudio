#ifndef DIALOGPBCD_H
#define DIALOGPBCD_H

#include <QDialog>

namespace Ui {
class DialogPBCD;
}
struct QUuid;

class DialogPBCD : public QDialog
{
    Q_OBJECT
private:
    explicit DialogPBCD(QWidget *parent = 0);
    ~DialogPBCD();

public:
    static void openPBCDDialog(QUuid projectID);

public slots:
    void onStartBinaryCD();
    void onStartFromToCD();

private:
    Ui::DialogPBCD *ui;

    QUuid prjID;
    QStringList imageLayerIDList;
};

#endif // DIALOGPBCD_H
