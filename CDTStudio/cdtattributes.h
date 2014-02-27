#ifndef CDTATTRIBUTES_H
#define CDTATTRIBUTES_H

#include <QtCore>

class CDTAttributes
{
public:
    friend QDataStream &operator <<(QDataStream &out,const CDTAttributes &attribute);
    friend QDataStream &operator >>(QDataStream &in, CDTAttributes &attribute);
    explicit CDTAttributes();
//    QString dbPath()const { return _dbPath;}
//    void setdabPath(QString &dbPath){ _dbPath = dbPath;}
private:
    QString dbPath;
};

QDataStream &operator <<(QDataStream &out,const CDTAttributes &attribute);
QDataStream &operator >>(QDataStream &in, CDTAttributes &attribute);
#endif // CDTATTRIBUTES_H
