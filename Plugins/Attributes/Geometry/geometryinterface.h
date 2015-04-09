#ifndef GEOMETRYINTERFACE_H
#define GEOMETRYINTERFACE_H

#include "cdtattributesinterface.h"


class GeometryInterface : public CDTAttributesInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTAttributesInterface" FILE "Geometry.json")
#else
    Q_INTERFACES(CDTAttributesInterface)
#endif // QT_VERSION >= 0x050000

public:
    GeometryInterface(QObject *parent = 0);
    ~GeometryInterface();

    QString attributesType() const;
    QString tableName() const;

    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal area(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal border_length(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal elongation(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal asymmetry(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal border_index(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal compactness(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal x_center(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal x_max(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal x_min(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal y_center(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal y_max(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal y_min(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal rectangular_fit(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal elliptic_fit(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal radius_of_largest_enclosed_ellipse(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal radius_of_smallest_enclosing_ellipse(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal shape_index(const AttributeParamsMultiBand &param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_ALL_BAND qreal roundness(const AttributeParamsMultiBand &param) const;

};

#endif // GEOMETRYINTERFACE_H
