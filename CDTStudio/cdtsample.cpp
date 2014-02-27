#include "cdtsample.h"

CDTSample::CDTSample()
{
}


QDataStream &operator<<(QDataStream &out, const CDTSample &classification)
{
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTSample &classification)
{
    return in;
}
