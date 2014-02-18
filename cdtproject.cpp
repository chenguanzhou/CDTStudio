#include "cdtproject.h"
#include "cdtprojecttreeitem.h"

CDTProject::CDTProject(QObject *parent):
    QObject(parent)
{
}

void CDTProject::setName(const QString &n)
{
    name = n;
}

void CDTProject::setPath(const QString &p)
{
    path = p;
}

void CDTProject::setimage(const QVector<CDTImageLayer> &m)
{
    for(int i=0;i<m.size();i++)
    {
        images.push_back(m[i]);
    }
}



QDataStream &operator <<(QDataStream &out,const CDTProject &project)
{
    out<<project.images<<project.isFileExsit<<project.name<<project.path;
    return out;
}


QDataStream &operator >>(QDataStream &in, CDTProject &project)
{
    in>>project.images>>project.isFileExsit>>project.name>>project.path;
    return in;
}
