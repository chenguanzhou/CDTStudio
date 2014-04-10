#ifndef FORMMST_H
#define FORMMST_H

#include <QWidget>
#include "mstmethodinterface.h"

namespace Ui {
class FormMST;
}

class FormMST : public QWidget
{
    Q_OBJECT
public:
    explicit FormMST(QWidget *parent = 0);
    ~FormMST();

    void setInterface(CDTSegmentationInterface* interface);
    QThread *thread();
    QMap<QString,QVariant> params() const;
private:
    Ui::FormMST *ui;    
    CDTSegmentationInterface* interface;
};

#endif // FORMMST_H
