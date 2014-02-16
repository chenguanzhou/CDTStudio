#include "cdtclassification.h"

CDTClassification::CDTClassification()
{

}


QDataStream &operator<<(QDataStream &out, const CDTClassification &classification)
{
    out<<classification.name<<classification.shapefilePath
      <<classification.method<<classification.params;
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTClassification &classification)
{
    in>>classification.name>>classification.shapefilePath
      >>classification.method>>classification.params;
    return in;
}



