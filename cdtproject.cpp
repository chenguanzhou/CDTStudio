#include "cdtproject.h"
#include "cdtprojecttreeitem.h"

CDTProject::CDTProject(QObject *parent):
    QObject(parent)
{
}

void CDTProject::addImageLayer(CDTImageLayer *image)
{
    images.push_back(image);
}

void CDTProject::addImageLayer(const QString name, const QString path)
{
    CDTImageLayer *image = new CDTImageLayer();
    image->setName(name);
    image->setPath(path);
    addImageLayer(image);
}

void CDTProject::setName(const QString &n)
{
    name = n;
}

void CDTProject::setPath(const QString &p)
{
    path = p;
}


QDataStream &operator <<(QDataStream &out,const CDTProject &project)
{
    out<<project.images.size();
    foreach (CDTImageLayer* image, project.images) {
        out<<*image;
    }
    out<<project.isFileExsit<<project.name<<project.path;
    return out;
}


QDataStream &operator >>(QDataStream &in, CDTProject &project)
{
    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTImageLayer* image = new CDTImageLayer();
        in>>*image;
        project.images.push_back(image);
    }
    in>>project.isFileExsit>>project.name>>project.path;
    return in;
}
