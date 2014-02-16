#include "cdtattributes.h"

CDTAttributes::CDTAttributes()
{

}


QDataStream &operator<<(QDataStream &out, const CDTAttributes &attribute)
{
    out<<attribute.dbPath;
    return out;
}


QDataStream &operator>>(QDataStream &in,CDTAttributes &attribute)
{
    in>>attribute.dbPath;
    return in;
}
