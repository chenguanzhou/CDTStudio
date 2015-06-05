#include "geometryinterface.h"
#include <limits>
#include <numeric>

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
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        mean_x +=pt.x();
        mean_y +=pt.y();
        mean_xy +=pt.x()*pt.y();
    });
    mean_x /=param.pointsVecF.size();
    mean_y /=param.pointsVecF.size();
    mean_xy /=param.pointsVecF.size();

    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        var_x +=(mean_x-pt.x())*(mean_x-pt.x());
        var_y+=(mean_y-pt.y())*(mean_y-pt.y());
        var_xy +=(mean_xy-pt.x()*pt.y())*(mean_xy-pt.x()*pt.y());
    });

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
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        refValue +=pt.x();
    });
    refValue /=param.pointsVecF.size();
    return refValue;
}

qreal GeometryInterface::x_max(const AttributeParamsMultiBand &param) const
{
    qreal refValue = param.pointsVecF[0].x();
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        qreal d = pt.x();
        if(d>refValue)
            refValue=d;
    });
    return refValue;
}

qreal GeometryInterface::x_min(const AttributeParamsMultiBand &param) const
{
    qreal refValue = param.pointsVecF[0].x();
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        qreal d = pt.x();
        if(d<refValue)
            refValue=d;
    });
    return refValue;
}

qreal GeometryInterface::y_center(const AttributeParamsMultiBand &param) const
{
    qreal refValue =0;
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        refValue +=pt.y();
    });
    refValue /= param.pointsVecF.size();
    return refValue;
}

qreal GeometryInterface::y_max(const AttributeParamsMultiBand &param) const
{
    qreal refValue = param.pointsVecF[0].y();
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        qreal d = pt.y();
        if(d>refValue)
            refValue=d;
    });
    return refValue;
}

qreal GeometryInterface::y_min(const AttributeParamsMultiBand &param) const
{
    qreal refValue = param.pointsVecF[0].y();
    std::for_each(param.pointsVecF.begin(),param.pointsVecF.end(),[&](const QPointF &pt){
        qreal d = pt.y();
        if(d<refValue)
            refValue=d;
    });
    return refValue;
}

qreal GeometryInterface::rectangular_fit(const AttributeParamsMultiBand &param) const
{
    double k =sqrt((param.area)/(param.longSideOfMBR*param.shortSideOfMBR));
    double rect_width =param.longSideOfMBR*k;
    double rect_highth =param.shortSideOfMBR*k;
    double x_min=0, y_min=0, x_max=0, y_max=0;

    qreal refValue =0;
    x_min =param.rotated_center.x()-0.5*rect_width;
    y_min =param.rotated_center.y()-0.5*rect_highth;
    x_max =param.rotated_center.x()+0.5*rect_width;
    y_max =param.rotated_center.y()+0.5*rect_highth;

    std::for_each(param.rotatedPointsVec.begin(),param.rotatedPointsVec.end(),[&](const QPointF &pt){
        if(x_min<=pt.x() &&pt.x()<=x_max
                &&y_min<=pt.y() && pt.y()<=y_max)
        ++refValue;
    });
    refValue /= param.rotatedPointsVec.size();
    return refValue;
}

qreal GeometryInterface::elliptic_fit(const AttributeParamsMultiBand &param) const
{
    qreal refValue =0;
    qreal a_1_2 = 1./(param.majorSemiAxesOfAE*param.majorSemiAxesOfAE);
    qreal b_1_2 = 1./(param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);
    const QPointF &center = param.rotated_center;
    std::for_each(param.rotatedPointsVec.begin(),param.rotatedPointsVec.end(),[&](const QPointF &pt){
        if(((pt.x()-center.x())*(pt.x()-center.x())*a_1_2 +
            (pt.y()-center.y())*(pt.y()-center.y())*b_1_2)<=1)
            ++refValue;
    });
    refValue /=param.rotatedPointsVec.size();
    return refValue;
}

qreal GeometryInterface::radius_of_largest_enclosed_ellipse(const AttributeParamsMultiBand &param) const
{
    qreal refValue=0;
    double MinK = std::numeric_limits<double>::max();
    qreal a_1_2 = 1./(param.majorSemiAxesOfAE*param.majorSemiAxesOfAE);
    qreal b_1_2 = 1./(param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);
    const QPointF &center = param.rotated_center;
    std::for_each(param.ringPointsVec.begin(),param.ringPointsVec.end(),[&](const QPointF &pt){
        double K =  (pt.x()-center.x())*(pt.x()-center.x())*a_1_2 +
                (pt.y()-center.y())*(pt.y()-center.y())*b_1_2;
        if(MinK>K)
            MinK=K;
    });
    refValue=sqrt(MinK);
    return refValue;
}

qreal GeometryInterface::radius_of_smallest_enclosing_ellipse(const AttributeParamsMultiBand &param) const
{
    qreal refValue =0;
    qreal a_1_2 = 1./(param.majorSemiAxesOfAE*param.majorSemiAxesOfAE);
    qreal b_1_2 = 1./(param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);

    const QPointF &center = param.rotated_center;
    std::for_each(param.rotatedPointsVec.begin(),param.rotatedPointsVec.end(),[&](const QPointF &pt){
        double k = (pt.x()-center.x())*(pt.x()-center.x())*a_1_2+
                (pt.y()-center.y())*(pt.y()-center.y())*b_1_2;
        if(refValue <=k)
            refValue=k;
    });
    return sqrt(refValue);
}

qreal GeometryInterface::roundness(const AttributeParamsMultiBand &param) const
{
    qreal refValue=0;
    qreal a_1_2 = 1./(param.majorSemiAxesOfAE*param.majorSemiAxesOfAE);
    qreal b_1_2 = 1./(param.minorSemiAxesOfAE*param.minorSemiAxesOfAE);
    const QPointF &center = param.rotated_center;

    double MinK=std::numeric_limits<double>::max();
    double MaxK=std::numeric_limits<double>::min();    

    std::for_each(param.ringPointsVec.begin(),param.ringPointsVec.end(),[&](const QPointF &pt){
        double K =  (pt.x()-center.x())*(pt.x()-center.x())*a_1_2 +
                (pt.y()-center.y())*(pt.y()-center.y())*b_1_2;
        if (MinK>K)MinK=K;
        if (MaxK<K)MaxK=K;
    });

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
