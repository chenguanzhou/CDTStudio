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

class CDTAttributesInterface: public QObject
{
    Q_OBJECT
public:
    explicit CDTAttributesInterface(QObject* parent = 0):QObject(parent)
    {
        qRegisterMetaType<QVector<QPoint> >("QVector<QPoint>");
        qRegisterMetaType<QVector<uchar*> >("QVector<uchar*>");
        qRegisterMetaType<uchar* >("uchar*");
    }

    virtual QString attributesType() const=0;
    virtual QList<AttributeMethod> attributesMethods()const=0;
    virtual QString tableName() const=0;
};

Q_DECLARE_INTERFACE(CDTAttributesInterface,"cn.edu.WHU.CDTStudio.CDTAttributesInterface/1.0")
#endif // CDTATTRIBUTESINTERFACE_H
