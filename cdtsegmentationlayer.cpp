#include "cdtsample.h"
#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"


CDTSegmentationLayer::CDTSegmentationLayer()
{
}

QDataStream &operator<<(QDataStream &out, const CDTSegmentationLayer &segmentation)
{
    out<<segmentation.name<<segmentation.shapefilePath<<segmentation.method
      <<segmentation.params<<segmentation.classifications<<segmentation.attributes
      <<segmentation.samples;
    return out;
}


QDataStream &operator>>(QDataStream &in,CDTSegmentationLayer &segmentation)
{
    in>>segmentation.name>>segmentation.shapefilePath>>segmentation.method
     >>segmentation.params>>segmentation.classifications
     >>segmentation.attributes>>segmentation.samples;
    return in;
}
