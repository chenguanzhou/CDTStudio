#include "mstmethodinterface.h"
#include "formmst.h"


MSTMethodInterface::MSTMethodInterface(QObject *parent)
    :CDTSegmentationInterface(parent)
{
//    frmMST->setInterface(this);
//    connect(frmMST,SIGNAL(finished()),this,SIGNAL(finished()));
}

MSTMethodInterface::~MSTMethodInterface()
{
}

QString MSTMethodInterface::segmentationMethod() const
{
    return QString("mst");
}

QWidget *MSTMethodInterface::paramsForm()
{
    FormMST *frmMST= new FormMST();
    frmMST->setInterface(this);
    connect(frmMST,SIGNAL(finished()),this,SIGNAL(finished()));
    return frmMST;
}

Q_EXPORT_PLUGIN2(MSTMethod, MSTMethodInterface)
