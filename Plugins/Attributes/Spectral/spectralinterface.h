#ifndef SPECTRALINTERFACE_H
#define SPECTRALINTERFACE_H

#include "../../../Interfaces/cdtattributesinterface.h"

class SpectralInterface : public CDTAttributesInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTAttributesInterface" FILE "Spectral.json")
#else
    Q_INTERFACES(CDTAttributesInterface)
#endif // QT_VERSION >= 0x050000

public:
    SpectralInterface(QObject *parent = 0);

    QString attributesType() const;
    QList<AttributeMethod> attributesMethods()const;
    QString tableName() const;

    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal brightness(
            QVector<QPoint> points,
            QVector<uchar*> buffer,
            int dataType,
            int nXSize,int nYSize) const;

    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal layer_mean(
            QVector<QPoint> points,
            uchar* buffer,
            int dataType,
            int nXSize,int nYSize) const;

    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal layer_stddev(
            QVector<QPoint> points,
            uchar* buffer,
            int dataType,
            int nXSize,int nYSize) const;

private:



};

#endif // SPECTRALINTERFACE_H
