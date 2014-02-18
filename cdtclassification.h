#ifndef CDTCLASSIFICATION_H
#define CDTCLASSIFICATION_H

#include <QtCore>
#include <QMap>
#include <QVariant>
#include "cdtprojecttreeitem.h"

class CDTClassification
{
public:
    explicit CDTClassification();
//    explicit CDTClassification(const QString &n,const QString &s,const QString &m);

    friend QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
    friend QDataStream &operator>>(QDataStream &in, CDTClassification &classification);
public:
    void updateTreeModel(CDTProjectTreeItem* parent);
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
