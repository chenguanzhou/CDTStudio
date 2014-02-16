#ifndef CDTSAMPLE_H
#define CDTSAMPLE_H

#include <QtCore>

class CDTSample
{
public:
    explicit CDTSample();
};
QDataStream &operator<<(QDataStream &out, const CDTSample &classification);
QDataStream &operator>>(QDataStream &in, CDTSample &classification);

#endif // CDTSAMPLE_H
