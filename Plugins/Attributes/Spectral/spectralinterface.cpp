#include "spectralinterface.h"


SpectralInterface::SpectralInterface(QObject *parent) :
    QGenericPlugin(parent)
{
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Spectral, SpectralInterface)
#endif // QT_VERSION < 0x050000
