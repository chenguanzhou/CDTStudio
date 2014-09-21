#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "cdtpbcddiffinterface.h"


class ImageDifferencing : public CDTPBCDDiffInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTPBCDDiffInterface" FILE "ImageDifferencing.json")
#else
    Q_INTERFACES(CDTPBCDDiffInterface)
#endif // QT_VERSION >= 0x050000

public:
    ImageDifferencing(QObject *parent = 0);

    QString methodName()const;
    QString generateDiffImage(QList<QPair<GDALRasterBand*,GDALRasterBand*> > poBands,GDALDataset* outDiffDS) ;
};

#endif // GENERICPLUGIN_H
