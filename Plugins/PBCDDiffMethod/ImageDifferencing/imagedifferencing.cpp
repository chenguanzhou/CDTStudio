#include "imagedifferencing.h"


ImageDifferencing::ImageDifferencing(QObject *parent) :
    CDTPBCDDiffInterface(parent)
{

}

QString ImageDifferencing::methodName() const
{
    return "Image Differencing";
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Differencing, ImageDifferencing)
#endif // QT_VERSION < 0x050000
