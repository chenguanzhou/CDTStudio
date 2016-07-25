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

    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        std::for_each(param.buffer.begin(),param.buffer.end(),[&](uchar* data){
            refValue += SRCVAL(data,param.dataType,pt.y() * param.nXSize + pt.x());
        });
    });

    return  refValue / (param.buffer.size()*param.pointsVecI.size());
}

qreal SpectralInterface::layer_mean(const AttributeParamsSingleBand& param) const
{
    qreal refValue =0;
    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        refValue += SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x());
    });
    return    refValue / param.pointsVecI.size();
}

qreal SpectralInterface::layer_stddev(const AttributeParamsSingleBand& param) const
{
    qreal Mean = 0;
    qreal refValue = 0;
    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        Mean += SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x());
    });
    Mean /= param.pointsVecI.size();

    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        refValue +=(Mean-SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x()))
                * (Mean-SRCVAL(param.buffer,param.dataType,pt.y()*param.nXSize + pt.x()));
    });
    return sqrt(refValue / param.pointsVecI.size());
}

qreal SpectralInterface::layer_median(const AttributeParamsSingleBand &param) const
{
    QList<qreal> refValueList;
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        qreal refValue;
        refValue = SRCVAL(param.buffer,param.dataType,param.pointsVecI[i].y() * param.nXSize + param.pointsVecI[i].x());
        refValueList.push_back(refValue);
    }
    qSort(refValueList.begin(),refValueList.end());
    return    refValueList.at(refValueList.size()/2);
}

qreal SpectralInterface::skewness(const AttributeParamsSingleBand &param) const
{
    qreal mean =0,var=0;
    qreal refValue=0;

    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        mean += SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x());
    });
    mean /= param.pointsVecI.size();

    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        qreal d = mean-SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x());
        var += d*d;
        refValue +=d*d*d;
    });

    refValue /=((sqrt(var))*(sqrt(var))*(sqrt(var)));
    return refValue;
}

qreal SpectralInterface::max_value(const AttributeParamsSingleBand &param) const
{
    qreal refValue =SRCVAL(param.buffer,param.dataType,param.pointsVecI[0].y() * param.nXSize + param.pointsVecI[0].x());
    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        qreal d = SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x());
        if(d > refValue)
            refValue=d;
    });
    return refValue;
}

qreal SpectralInterface::min_value(const AttributeParamsSingleBand &param) const
{
    qreal refValue =SRCVAL(param.buffer,param.dataType,param.pointsVecI[0].y() * param.nXSize + param.pointsVecI[0].x());
    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        qreal d = SRCVAL(param.buffer,param.dataType,pt.y() * param.nXSize + pt.x());
        if(d < refValue)
            refValue=d;
    });
    return refValue;
}

qreal SpectralInterface::mean_of_inner_border(const AttributeParamsSingleBand &param) const
{
    qreal refValue =0;
    int borderPointCount=0;

    QSet<QPoint> pointsSet;
    QVector<bool> maskBuffer(param.nXSize*param.nYSize,false);
    std::for_each(param.pointsVecI.begin(),param.pointsVecI.end(),[&](const QPoint &pt){
        maskBuffer[pt.y() * param.nXSize + pt.x()] = true;
    });

    for (int i=0;i<param.nYSize;++i)
    {
        bool isMask = maskBuffer.at(i*param.nXSize);
        if (isMask)
            pointsSet.insert(QPoint(0,i));
        for (int j=1;j<param.nXSize-1;++j)
        {
            bool current = maskBuffer.at(i*param.nXSize+j);
            if (isMask==current)
                continue;
            else if(isMask)
                pointsSet.insert(QPoint(j-1,i));
            else
                pointsSet.insert(QPoint(j,i));

            isMask = current;
        }
        if (maskBuffer.at(i*param.nXSize+param.nXSize-1))
            pointsSet.insert(QPoint(param.nXSize-1,i));
    }

    for (int i=0;i<param.nXSize;++i)
    {
        bool isMask = maskBuffer.at(i);
        if (isMask)
            pointsSet.insert(QPoint(i,0));
        for (int j=1;j<param.nYSize-1;++j)
        {
            bool current = maskBuffer.at(j*param.nXSize+i);
            if (isMask==current)
                continue;
            else if (isMask)
                pointsSet.insert(QPoint(i,j-1));
            else
                pointsSet.insert(QPoint(i,j));
            isMask = current;
        }
        if (maskBuffer.at((param.nYSize-1)*param.nXSize+i))
            pointsSet.insert(QPoint(i,param.nYSize-1));
    }

    std::for_each(pointsSet.begin(),pointsSet.end(),[&](const QPoint &pt){
        refValue += SRCVAL(param.buffer, param.dataType,pt.y() * param.nXSize + pt.x());
        ++borderPointCount;
    });


    refValue /= borderPointCount;
    return refValue;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Spectral, SpectralInterface)
#endif // QT_VERSION < 0x050000
