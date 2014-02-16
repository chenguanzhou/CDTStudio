#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include <QtCore>
#include <QVector>

#include "cdtimagelayer.h"

class CDTProject
{
public:
    explicit CDTProject();
    friend QDataStream &operator <<(QDataStream &out,const CDTProject &project);
    friend QDataStream &operator >>(QDataStream &in, CDTProject &project);

private:
    QString name;
    QString path;
    bool    isFileExsit;
    QVector<CDTImageLayer> images;
};
//QDataStream &operator <<(QDataStream &out,const CDTProject &project);
//QDataStream &operator >>(QDataStream &in, CDTProject &project);
#endif // CDTPROJECT_H
