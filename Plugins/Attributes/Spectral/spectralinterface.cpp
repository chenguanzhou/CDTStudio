#include "spectralinterface.h"
#include <gdal_priv.h>
#include<set>

uint qHash(const QPoint &point)
{
    QString valueStr(QString::number(point.x()) + QString::number(point.y()));
    return valueStr.toUInt();
}


SpectralInterface::SpectralInterface(QObject *parent) :
    CDTAttributesInterface(parent)
{
}

QString SpectralInterface::attributesType() const
{
    return tr("Spectral");
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
    QVector<QPoint> inner_border;
    QSet<QPoint> pointsSet;
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        pointsSet.insert(param.pointsVecI[i]);
    }
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        int x=param.pointsVecI[i].x();
        int y=param.pointsVecI[i].y();
        QSet<QPoint>::iterator end=pointsSet.end();

        if((pointsSet.find(QPoint(x,y-1))==end || pointsSet.find(QPoint(x,y+1))==end
            || pointsSet.find(QPoint(x+1,y))==end || pointsSet.find(QPoint(x-1,y))==end))
        {
            inner_border.push_back(param.pointsVecI[i]);
        }
    }
    for(unsigned int i=0;i<inner_border.size();++i)
    {
        refValue += SRCVAL(param.buffer,param.dataType,inner_border[i].y() * param.nXSize + inner_border[i].x());
    }
    refValue /=inner_border.size();
    return refValue  ;
}

qreal SpectralInterface::mean_of_outer_border(const AttributeParamsSingleBand &param) const
{
    qreal refValue =0;
    int num=0;
    QSet<QPoint> setTag;
    for(int i=0;i<param.pointsVecI.size();++i)
    {
        setTag.insert(param.pointsVecI[i]);
    }
    QPoint arr[4];
    int arr1[4]={0,0,0,0};
    for(QVector<QPoint>::size_type ix=0;ix!=param.pointsVecI.size();ix++)
    {
        arr[0].setX(param.pointsVecI[ix].x());
        arr[0].setY(param.pointsVecI[ix].y()-1);
        arr[1].setX(param.pointsVecI[ix].x()-1);
        arr[1].setY(param.pointsVecI[ix].y());
        arr[2].setX(param.pointsVecI[ix].x()+1);
        arr[2].setY(param.pointsVecI[ix].y());
        arr[3].setX(param.pointsVecI[ix].x());
        arr[3].setY(param.pointsVecI[ix].y()+1);
        for(size_t i=0;i<4;i++)
        {
            if(setTag.find(arr[i]) == setTag.end())
            {
                arr1[i] =0;
            }
            else
                arr1[i]=1;
//            arr1[i]=setTag.count();
        }
        if(!arr1[0]||!arr1[1]||!arr1[2]||!arr1[3])
        {
            refValue += param.buffer[param.pointsVecI[ix].y() * param.nXSize + param.pointsVecI[ix].x()];
            num++;
        }
    }
    refValue /=num;
    return refValue;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Spectral, SpectralInterface)
#endif // QT_VERSION < 0x050000
