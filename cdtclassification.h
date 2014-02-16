#ifndef CDTCLASSIFICATION_H
#define CDTCLASSIFICATION_H

#include <QtCore>
#include <QMap>
#include <QVariant>

class CDTClassification
{
public:
    explicit CDTClassification();
    friend QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
    friend QDataStream &operator>>(QDataStream &in, CDTClassification &classification);
public:
//    QString name()const{return _name;}
//    QString shapefilePath()const{return _shapefilePath;}
//    QString method()const{return _method;}
//    QMap<QString,QVariant> params()const{return _params;}

private:
    QString name;
    QString shapefilePath;
    QString method;
    QMap<QString,QVariant> params;
};

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

#endif // CDTCLASSIFICATION_H
