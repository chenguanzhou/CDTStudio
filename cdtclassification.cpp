#include "cdtclassification.h"

CDTClassification::CDTClassification(const QString &name, const QString &shapefilePath, const QString method, const QMap<QString, QVariant> &params)
    :_name(name),_shapefilePath(shapefilePath),_method(method),_params(params)
{

}


QDataStream &operator<<(QDataStream &out, const CDTClassification &classification)
{
    out<<classification.name()<<classification.shapefilePath()
      <<classification.method()<<classification.params();
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTClassification &classification)
{
    QString name;
    QString shapefilePath;
    QString method;
    QMap<QString,QVariant> params;
    in>>name>>shapefilePath>>method>>params;
    classification = CDTClassification(name,shapefilePath,method,params);
    return in;
}



