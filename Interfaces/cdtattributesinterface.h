#ifndef CDTATTRIBUTESINTERFACE_H
#define CDTATTRIBUTESINTERFACE_H

#include <QtCore>

#define CDT_ATTRIBUTE_ALL_BAND
#define CDT_ATTRIBUTE_SINGLE_BAND
#define CDT_ATTRIBUTE_SINGLE_BAND_ANGLE
#define CDT_ATTRIBUTE_CUSTOM

class AttributeMethod
{
public:
    AttributeMethod(const QString &name,const QString &type):methodName(name),methodType(type){}
    QString methodName;
    QString methodType;
};

class TextureParam
{
public:
    uchar* buf;
    int angle;
    double minVal;
    double maxVal;
};

template <class T>
class AttributeParams
{
public:
    AttributeParams(){}
    AttributeParams(
            const QVector<QPoint> &ptVecI,
            const T &buf,
            int dt,
            int nx,
            int ny,
            const QVector<QPointF> &ptVecF,
            const QVector<QPointF> &rotPtVec,
            const QVector<QPointF> &ringPtVec,
            double areaSize,
            double borLength,
            double lgSideOfMBR,
            double stSideOfMBR,
            double majSemiAxesOfAE,
            double minSemiAxesOfAE,
            const QPointF rotCenter):
        pointsVecI(ptVecI),
        buffer(buf),
        dataType(dt),
        nXSize(nx),
        nYSize(ny),
        pointsVecF(ptVecF),
        rotatedPointsVec(rotPtVec),
        ringPointsVec(ringPtVec),
        area(areaSize),
        borderLength(borLength),
        longSideOfMBR(lgSideOfMBR),
        shortSideOfMBR(stSideOfMBR),
        majorSemiAxesOfAE(majSemiAxesOfAE),
        minorSemiAxesOfAE(minSemiAxesOfAE),
        rotated_center(rotCenter){}

    //Image Coordinate
    QVector<QPoint> pointsVecI;
    T buffer;
    int dataType;
    int nXSize;
    int nYSize;

    //Spatial Coordinate
    QVector<QPointF> pointsVecF;
    QVector<QPointF> rotatedPointsVec;
    QVector<QPointF> ringPointsVec;
    double area;
    double borderLength;
    double longSideOfMBR;
    double shortSideOfMBR;
    double majorSemiAxesOfAE;
    double minorSemiAxesOfAE;
    QPointF rotated_center;
};

typedef AttributeParams<QVector<uchar*> > AttributeParamsMultiBand;
typedef AttributeParams<uchar*> AttributeParamsSingleBand;
typedef AttributeParams<TextureParam> AttributeParamsSingleAngleBand;

class CDTAttributesInterface: public QObject
{
    Q_OBJECT
public:
    explicit CDTAttributesInterface(QObject* parent = 0):QObject(parent)
    {
        qRegisterMetaType<AttributeParamsSingleBand>("AttributeParamsSingleBand");
        qRegisterMetaType<AttributeParamsMultiBand>("AttributeParamsMultiBand");
    }

    virtual ~CDTAttributesInterface(){}
    virtual QString attributesType() const=0;
    virtual QString tableName() const=0;
    virtual QStringList attributesName(QString name,QString funcName)
    {
        Q_UNUSED(funcName);
        return QStringList()<<name;
    }

    QList<AttributeMethod> attributesMethods()const
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
};



Q_DECLARE_INTERFACE(CDTAttributesInterface,"cn.edu.WHU.CDTStudio.CDTAttributesInterface/1.0")
#endif // CDTATTRIBUTESINTERFACE_H
