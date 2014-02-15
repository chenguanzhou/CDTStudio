#ifndef CDTCLASSIFICATION_H
#define CDTCLASSIFICATION_H

#include <QtCore>
#include <QMap>
#include <QVariant>

class CDTClassification
{
public:
    explicit CDTClassification(const QString &name,
                               const QString &shapefilePath,
                               const QString method,
                               const QMap<QString,QVariant> &params);

public:
    QString name()const{return _name;}
    QString shapefilePath()const{return _shapefilePath;}
    QString method()const{return _method;}
    QMap<QString,QVariant> params()const{return _params;}

private:
    QString _name;
    QString _shapefilePath;
    QString _method;
    QMap<QString,QVariant> _params;
};

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

#endif // CDTCLASSIFICATION_H
