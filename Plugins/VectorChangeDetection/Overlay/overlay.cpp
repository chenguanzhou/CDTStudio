#include "overlay.h"

Overlay::Overlay(QObject *parent)
    :CDTVectorChangeDetectionInterface(parent)
{
}

Overlay::~Overlay()
{
}

QString Overlay::methodName() const
{
    return tr("Overlay");
}

void Overlay::startDetection(QProgressBar *progressBar, QLabel *label)
{

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Overlay, Overlay)
#endif // QT_VERSION < 0x050000
