#include "spectralinterface.h"
#include <gdal_priv.h>
#include <QDebug>

uint qHash(const QPoint &point)
{
    return ((qint16)point.x()) | point.y()<<16;
}


SpectralInterface::SpectralInterface(QObject *parent) :
    CDTAttributesInterface(parent)
{
}

SpectralInterface::~SpectralInterface()
{
}

QString SpectralInterface::attributesType() const
{
    return tr("Spectral");
}

QString SpectralInterface::tableName() const
{
    return QString("Spectral");
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

qreal SpectralInterface::skewness(const AttributeParamsSingleBand &param) const
{
    qreal mean =0,var=0;
    qreal refValue=0;
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        mean += SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
    }
    mean /= param.pointsVecI.size();
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        var +=(mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x()))
                * (mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y()*param.nXSize + param.pointsVecI[i].x()));
    }
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        refValue +=(mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x()))
                * (mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y ()* param.nXSize + param.pointsVecI[i].x()))
                *(mean-SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x()));
    }
    refValue /=((sqrt(var))*(sqrt(var))*(sqrt(var)));
    return refValue;
}

qreal SpectralInterface::max_value(const AttributeParamsSingleBand &param) const
{
    qreal refValue =0;
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        if(SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x()) >= refValue)
            refValue=SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
    }
    return refValue;
}

qreal SpectralInterface::min_value(const AttributeParamsSingleBand &param) const
{
    qreal refValue =SRCVAL(param.buffer,param.dataType,param.pointsVecI[1].y() * param.nXSize + param.pointsVecI[1].x());
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        if(SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x()) <= refValue)
            refValue=SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
    }
    return refValue;
}



qreal SpectralInterface::mean_of_inner_border(const AttributeParamsSingleBand &param) const
{
    qreal refValue =0;
    QSet<QPoint> pointsSet;
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        pointsSet.insert(param.pointsVecI[i]);
    }

    int borderPointCount=0;
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        int x=param.pointsVecI[i].x();
        int y=param.pointsVecI[i].y();

        if((!pointsSet.contains(QPoint(x,y-1))) || (!pointsSet.contains(QPoint(x,y+1)))
                || (!pointsSet.contains(QPoint(x+1,y))) || (!pointsSet.contains(QPoint(x-1,y))))
        {
            refValue += SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
            ++borderPointCount;
        }
    }
    refValue /=borderPointCount;
    return refValue;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Spectral, SpectralInterface)
#endif // QT_VERSION < 0x050000
