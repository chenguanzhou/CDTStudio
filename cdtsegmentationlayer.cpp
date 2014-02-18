#include "cdtsample.h"
#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include <QList>


CDTSegmentationLayer::CDTSegmentationLayer()
{
}

//CDTSegmentationLayer::CDTSegmentationLayer(const QString &n, const QString &s, const QString &m):
//    name(n),shapefilePath(s),method(m)
//{
//    params["threshold"] =25;
//    params["minArea"] =100;

//    QVector<CDTClassification> c;
//    c.push_back(CDTClassification("cls1","c:/","knn"));
//    c.push_back(CDTClassification("cls2","c:/","knn"));
//    setclassifications(c);
//}

void CDTSegmentationLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *segment =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,name);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,QObject::tr("shapefilePath"));
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,shapefilePath);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,QObject::tr("method"));
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,method);
    CDTProjectTreeItem *classificationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION_ROOT,QObject::tr("classifications"));

    for(int i=0;i<params.size();++i)
    {
        QList<QString> keys =params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,keys[i]);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,params[keys[i]].toString());
        methodroot->setChild(i,0,methodparam);
        methodroot->setChild(i,1,methodvalue);
    }

    segment->setChild(0,0,param);
    segment->setChild(0,1,value);
    segment->setChild(1,0,methodroot);
    segment->setChild(1,1,methodrootvalue);
    segment->setChild(2,classificationsroot);

    parent->appendRow(segment);

    for(int i=0;i<classifications.size();++i)
    {
        classifications[i].updateTreeModel(classificationsroot);
    }


}

//void CDTSegmentationLayer::setclassifications(QVector<CDTClassification> &c)
//{
//    for(int i=0;i<c.size();++i)
//    {
//        classifications.push_back(c[i]);
//    }
//}

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
