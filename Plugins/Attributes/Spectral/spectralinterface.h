#ifndef SPECTRALINTERFACE_H
#define SPECTRALINTERFACE_H

#include "cdtattributesinterface.h"


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
    ~SpectralInterface();

    QString attributesType() const;   
    QString tableName() const;

    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal brightness(const AttributeParamsMultiBand &param) const;

    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal layer_mean(
            const AttributeParamsSingleBand& param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal layer_stddev(
            const AttributeParamsSingleBand& param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal skewness(
            const AttributeParamsSingleBand& param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal max_value(
            const AttributeParamsSingleBand& param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal min_value(
            const AttributeParamsSingleBand& param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND qreal mean_of_inner_border(
            const AttributeParamsSingleBand& param) const;
};

#endif // SPECTRALINTERFACE_H
