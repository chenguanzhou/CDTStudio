#ifndef DIALOGPBCDADDBANDPAIR_H
#define DIALOGPBCDADDBANDPAIR_H

#include <QDialog>

namespace Ui {
class DialogPBCDAddBandPair;
}

class DialogPBCDAddBandPair : public QDialog
{
    Q_OBJECT

public:
    static QString getNewPair(QString path1, QString path2);

private:
    explicit DialogPBCDAddBandPair(QString path1, QString path2, QWidget *parent = 0);
    ~DialogPBCDAddBandPair();

    void setListWidget(int bandCount, QListWidget *widget);
    Ui::DialogPBCDAddBandPair *ui;
};

#endif // DIALOGPBCDADDBANDPAIR_H
