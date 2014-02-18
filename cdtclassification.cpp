#include "cdtclassification.h"

CDTClassification::CDTClassification()
{

}

//CDTClassification::CDTClassification(const QString &n, const QString &s, const QString &m):
//    name(n),shapefilePath(s),method(m)
//{
//    params["K"] =100;
//}

void CDTClassification::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *classification =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,name);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,QObject::tr("shapefilePath"));
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,shapefilePath);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,QObject::tr("method"));
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,method);

    for(int i=0;i<params.size();++i)
    {
        QList<QString> keys =params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,keys[i]);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,params[keys[i]].toString());
        methodroot->setChild(i,0,methodparam);
        methodroot->setChild(i,1,methodvalue);
    }

    classification->setChild(0,0,param);
    classification->setChild(0,1,value);
    classification->setChild(1,0,methodroot);
    classification->setChild(1,1,methodrootvalue);

    parent->appendRow(classification);
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



