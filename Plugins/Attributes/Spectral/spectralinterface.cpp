#include "spectralinterface.h"
#include <gdal_priv.h>




SpectralInterface::SpectralInterface(QObject *parent) :
    CDTAttributesInterface(parent)
{
}

QString SpectralInterface::attributesType() const
{
    return tr("Spectral");
}

QList<AttributeMethod> SpectralInterface::attributesMethods() const
{
    QList<AttributeMethod> methods;
    const QMetaObject *metaObject = this->metaObject();
    for (int i=0;i<metaObject->methodCount();++i)
    {
        QMetaMethod metaMethod = metaObject->method(i);
        if (QString(metaMethod.tag()) == QString("CDT_ATTRIBUTE_ALL_BAND") ||
                QString(metaMethod.tag()) == QString("CDT_ATTRIBUTE_SINGLE_BAND")||
                QString(metaMethod.tag()) == QString("CDT_ATTRIBUTE_SINGLE_BAND_ANGLE")||
                QString(metaMethod.tag()) == QString("CDT_ATTRIBUTE_CUSTOM"))
        {
            QString name = (metaMethod.signature());
            name = name.left(name.indexOf("("));
            methods<<AttributeMethod(name,metaMethod.tag());
        }

    }
    return methods;
}

QString SpectralInterface::tableName() const
{
    return QString("ObjectID_Spectral");
}

qreal SpectralInterface::brightness(const AttributeParamsMultiBand &param) const
{
    qreal refValue = 0;    
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        for (int k=0;k<param.buffer.size();++k)
        {
            refValue += SRCVAL(param.buffer[k],param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
        }
    }    
    return  refValue / (param.buffer.size()*param.pointsVecI.size());
}

qreal SpectralInterface::layer_mean(const AttributeParamsSingleBand& param) const
{
    qreal refValue =0;
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        refValue += SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
    }
    return    refValue / param.pointsVecI.size();
}

qreal SpectralInterface::layer_stddev(const AttributeParamsSingleBand& param) const
{
    qreal Mean = 0;
    qreal refValue = 0;
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        Mean += SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
    }
    Mean /= param.pointsVecI.size();
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        refValue +=(Mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x()))
                * (Mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y()*param.nXSize + param.pointsVecI[i].x()));
    }
    return sqrt(refValue / param.pointsVecI.size());
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Spectral, SpectralInterface)
#endif // QT_VERSION < 0x050000
