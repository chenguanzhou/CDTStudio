#include "cdtimagelayer.h"

CDTImageLayer::CDTImageLayer()
{
}

void CDTImageLayer::updateTreeModel(CDTProjectTreeItem *parent)
{

}

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image)
{
    out<<image.path<<image.segmentations;
    return out ;
}


QDataStream &operator>>(QDataStream &in, CDTImageLayer &image)
{
    in>>image.path>>image.segmentations;
    return in;
}
