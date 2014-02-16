#include "cdtproject.h"

CDTProject::CDTProject()
{
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
