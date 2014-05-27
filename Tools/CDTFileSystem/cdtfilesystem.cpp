#include "cdtfilesystem.h"
#include <QtCore>

CDTFileSystem::CDTFileSystem()
{

}

CDTFileSystem::~CDTFileSystem()
{

}


QDataStream &operator<<(QDataStream &out, const CDTFileSystem &files)
{
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTFileSystem &files)
{
    return in;
}
