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
    //    connect(frmMST,SIGNAL(finished(QMap<QString, QVariant>)),this,SIGNAL(finished(QMap<QString, QVariant>)));
    return frmMST;
}

QMap<QString, QVariant> MSTMethodInterface::params(QWidget *form)
{
    return ((FormMST*)form)->params();
}

QThread *MSTMethodInterface::thread(QWidget *form)
{
    if (form==NULL) return NULL;

    FormMST *frmMST=(FormMST *)form;
    return frmMST->thread();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MSTMethod, MSTMethodInterface)
#endif // QT_VERSION < 0x050000
