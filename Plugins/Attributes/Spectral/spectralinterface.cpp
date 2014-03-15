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

qreal SpectralInterface::brightness(QVector<QPoint> points, QVector<uchar *> buffer, int dataType, int nXSize, int nYSize) const
{
    qreal refValue = 0;
    for (int i=0;i<points.size();++i)
    {
        for (int k=0;k<buffer.size();++k)
        {
            refValue += SRCVAL(buffer[k],dataType,points[i].y() * nXSize + points[i].x());
        }
    }
    return  refValue / (buffer.size()*points.size());
}

qreal SpectralInterface::layer_mean(QVector<QPoint> points, uchar *buffer, int dataType, int nXSize, int nYSize) const
{
    qreal refValue =0;
    for (int i=0;i<points.size();++i)
    {
        refValue += SRCVAL(buffer,dataType,points[i].y() * nXSize + points[i].x());
    }
    return    refValue / points.size();
}

qreal SpectralInterface::layer_stddev(QVector<QPoint> points, uchar *buffer, int dataType, int nXSize, int nYSize) const
{
    qreal Mean = 0;
    qreal refValue;
    for (int i=0;i<points.size();++i)
    {
        Mean += SRCVAL(buffer,dataType,points[i].y() * nXSize + points[i].x());
    }
    Mean /= points.size();
    for (int i=0;i<points.size();++i)
    {
        refValue +=(Mean-SRCVAL(buffer,dataType,points[i].y() * nXSize + points[i].x()))
                * (Mean-SRCVAL(buffer,dataType,points[i].y()*nXSize + points[i].x()));
    }
    return sqrt(refValue / points.size());
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Spectral, SpectralInterface)
#endif // QT_VERSION < 0x050000
