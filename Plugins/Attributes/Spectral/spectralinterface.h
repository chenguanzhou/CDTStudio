#ifndef SPECTRALINTERFACE_H
#define SPECTRALINTERFACE_H

#include <QGenericPlugin>


class SpectralInterface : public QGenericPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "Spectral.json")
#endif // QT_VERSION >= 0x050000

public:
    SpectralInterface(QObject *parent = 0);
};

#endif // SPECTRALINTERFACE_H
