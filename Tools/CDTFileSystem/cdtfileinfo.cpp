#include "cdtfileinfo.h"


QDataStream &operator<<(QDataStream &out, const CDTFileInfo &fileInfo)
{
    out<<fileInfo.prifix<<fileInfo.suffix<<fileInfo.path<<fileInfo.affiliatedFiles;
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTFileInfo &fileInfo)
{
    in>>fileInfo.prifix>>fileInfo.suffix>>fileInfo.path>>fileInfo.affiliatedFiles;
    return in;
}
