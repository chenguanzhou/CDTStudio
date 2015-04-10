#include "geometryinterface.h"
#include <limits>

GeometryInterface::GeometryInterface(QObject *parent) :
    CDTAttributesInterface(parent)
{
}

GeometryInterface::~GeometryInterface()
{

}

QString GeometryInterface::attributesType() const
{
    return tr("Geometry");
}

QString GeometryInterface::tableName() const
{
    return QString("Geometry");
}

qreal  GeometryInterface::area(const AttributeParamsMultiBand &param) const
{
    return param.area;
}

qreal GeometryInterface::border_length(const AttributeParamsMultiBand &param) const
{
    return param.borderLength;
}

qreal GeometryInterface::elongation(const AttributeParamsMultiBand &param) const
{
    qreal refValue;
    if(param.longSideOfMBR>=param.shortSideOfMBR)
        refValue = param.longSideOfMBR / param.shortSideOfMBR;
    else {
        refValue = param.shortSideOfMBR / param.longSideOfMBR;
    }
    return refValue;
}

qreal GeometryInterface::asymmetry(const AttributeParamsMultiBand &param) const
{
    qreal refValue;
    double mean_x =0,mean_y =0,mean_xy =0;
    double var_x =0,var_y =0,var_xy =0;
    for(int i =0; i< param.pointsVecF.size(); ++i)
    {
        mean_x +=param.pointsVecF[i].x();
        mean_y +=param.pointsVecF[i].y();
        mean_xy +=param.pointsVecF[i].x()*param.pointsVecF[i].y();
    }
    mean_x /=param.pointsVecF.size();
    mean_y /=param.pointsVecF.size();
    mean_xy /=param.pointsVecF.size();
    for(int i=0;i< param.pointsVecF.size();++i)
    {
        var_x +=(mean_x-param.pointsVecF[i].x())*(mean_x-param.pointsVecF[i].x());
        var_y+=(mean_y-param.pointsVecF[i].y())*(mean_y-param.pointsVecF[i].y());
        var_xy +=(mean_xy-param.pointsVecF[i].x()*param.pointsVecF[i].y())*(mean_xy-param.pointsVecF[i].x()*param.pointsVecF[i].y());
    }

    refValue = 2*sqrt(0.25*(var_x+var_y)*(var_x+var_y)+var_xy*var_xy-var_x*var_y)/(var_x+var_y);
    return refValue;
}

qreal GeometryInterface::border_index(const AttributeParamsMultiBand &param) const
{
    qreal refValue;
    refValue = param.borderLength/(2*(param.longSideOfMBR+param.shortSideOfMBR));
    return refValue;
}

qreal GeometryInterface::compactness(const AttributeParamsMultiBand &param) const
{
    qreal refValue;
    refValue = param.longSideOfMBR*param.shortSideOfMBR/param.area;
    return refValue;
}

qreal GeometryInterface::x_center(const AttributeParamsMultiBand &param) const
{
    qreal refValue=0;
    for(int i=0;i<param.pointsVecF.size();++i)
    {
        refValue +=param.pointsVecF[i].x();
    }
    refValue /=param.pointsVecF.size();
    return refValue;
}

qreal GeometryInterface::x_max(const AttributeParamsMultiBand &param) const
{
    qreal refValue = 0;
    for(int i=0;i<param.pointsVecF.size();++i)
    {
        if(param.pointsVecF[i].x()>=refValue)
            refValue=param.pointsVecF[i].x();
    }
    return refValue;
}

qreal GeometryInterface::x_min(const AttributeParamsMultiBand &param) const
{
    qreal refValue = param.pointsVecF[1].x();
    for(int i=0;i<param.pointsVecF.size();++i)
    {
        if(param.pointsVecF[i].x()<=refValue)
            refValue=param.pointsVecF[i].x();
    }
    return refValue;
}

qreal GeometryInterface::y_center(const AttributeParamsMultiBand &param) const
{
    qreal refValue =0;
    for(int i=0;i<param.pointsVecF.size();++i)
    {
        refValue +=param.pointsVecF[i].y();
    }
    refValue /= param.pointsVecF.size();
    return refValue;
}

qreal GeometryInterface::y_max(const AttributeParamsMultiBand &param) const
{
    qreal refValue = 0;
    for(int i=0;i<param.pointsVecF.size();++i)
    {
        if(param.pointsVecF[i].y() >=refValue)
            refValue=param.pointsVecF[i].y();
    }
    return refValue;
}

qreal GeometryInterface::y_min(const AttributeParamsMultiBand &param) const
{
    qreal refValue = param.pointsVecF[1].y();
    for(int i=0;i<param.pointsVecF.size();++i)
    {
        if(param.pointsVecF[i].y() <=refValue)
            refValue=param.pointsVecF[i].y();
    }
    return refValue;
}

qreal GeometryInterface::rectangular_fit(const AttributeParamsMultiBand &param) const
{
    double k =sqrt((param.area)/(param.longSideOfMBR*param.shortSideOfMBR));
    double rect_width =param.longSideOfMBR*k;
    double rect_highth =param.shortSideOfMBR*k;
    double x_min=0, y_min=0, x_max=0, y_max=0;

    qreal refValue =0;
    x_min =param.rotated_center.x()-(1./2.)*rect_width;
    y_min =param.rotated_center.y()-(1./2.)*rect_highth;
    x_max =param.rotated_center.x()+(1./2.)*rect_width;
    y_max =param.rotated_center.y()+(1./2.)*rect_highth;

    for(int i=0;i<param.rotatedPointsVec.size();++i)
    {
        if(x_min<=param.rotatedPointsVec[i].x() &&param.rotatedPointsVec[i].x()<=x_max
                &&y_min<=param.rotatedPointsVec[i].y() && param.rotatedPointsVec[i].y()<=y_max)
            ++refValue;
    }
    refValue /=param.rotatedPointsVec.size();
    return refValue;
}

qreal GeometryInterface::elliptic_fit(const AttributeParamsMultiBand &param) const
{
    qreal refValue =0;
    for(int i=0;i<param.rotatedPointsVec.size();++i)
    {
        if((param.rotatedPointsVec[i].x()-param.rotated_center.x())*(param.rotatedPointsVec[i].x()-param.rotated_center.x())
                /(param.majorSemiAxesOfAE*param.majorSemiAxesOfAE)
                +(param.rotatedPointsVec[i].y()-param.rotated_center.y())*(param.rotatedPointsVec[i].y()-param.rotated_center.y())
                /(param.minorSemiAxesOfAE*param.minorSemiAxesOfAE)<=1)

        {
            ++refValue;
        }
    }
    refValue /=param.rotatedPointsVec.size();
    return refValue;
}

qreal GeometryInterface::radius_of_largest_enclosed_ellipse(const AttributeParamsMultiBand &param) const
{
    qreal refValue=0;
    double MinK = std::numeric_limits<double>::max();
    for(int ix=0;ix!=param.ringPointsVec.size();ix++)
    {
        double K =  (param.ringPointsVec[ix].x()-param.rotated_center.x())*(param.ringPointsVec[ix].x()-param.rotated_center.x())
                / (param.majorSemiAxesOfAE*param.majorSemiAxesOfAE)
                +   (param.ringPointsVec[ix].y()-param.rotated_center.y())*(param.ringPointsVec[ix].y()-param.rotated_center.y())
                / (param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);
        if(MinK>K)
            MinK=K;
    }
    refValue=sqrt(MinK);
    return refValue;
}

qreal GeometryInterface::radius_of_smallest_enclosing_ellipse(const AttributeParamsMultiBand &param) const
{
    qreal refValue =0;
    for(int i=0;i< param.rotatedPointsVec.size();++i)
    {
        double k = (param.rotatedPointsVec[i].x()-param.rotated_center.x())*(param.rotatedPointsVec[i].x()-param.rotated_center.x())/(param.majorSemiAxesOfAE*param.majorSemiAxesOfAE)+
                (param.rotatedPointsVec[i].y()-param.rotated_center.y())*(param.rotatedPointsVec[i].y()-param.rotated_center.y())/(param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);
        if(refValue <=k)
            refValue=k;
    }
    refValue=sqrt(refValue);
    return refValue;
}

qreal GeometryInterface::roundness(const AttributeParamsMultiBand &param) const
{
    qreal refValue=0;
    double MinK=std::numeric_limits<double>::max();
    double MaxK=std::numeric_limits<double>::min();

    for(int ix=0;ix!=param.ringPointsVec.size();ix++)
    {
        double K =  (param.ringPointsVec[ix].x()-param.rotated_center.x())*(param.ringPointsVec[ix].x()-param.rotated_center.x())
                / (param.majorSemiAxesOfAE*param.majorSemiAxesOfAE)
                +   (param.ringPointsVec[ix].y()-param.rotated_center.y())*(param.ringPointsVec[ix].y()-param.rotated_center.y())
                / (param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);
        if (MinK>K)
            MinK=K;
        if (MaxK<K)
            MaxK=K;
    }

    refValue = sqrt(MaxK) - sqrt(MinK);
    return refValue;
}

qreal GeometryInterface::shape_index(const AttributeParamsMultiBand &param) const
{
    qreal refValue=0;
    double borderLength_of_square=4*sqrt(param.area);
    refValue=param.borderLength/borderLength_of_square;
    return refValue;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Geometry, GeometryInterface)
#endif // QT_VERSION < 0x050000
