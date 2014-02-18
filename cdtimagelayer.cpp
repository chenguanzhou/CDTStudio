#include "cdtimagelayer.h"

CDTImageLayer::CDTImageLayer()
{
}

//CDTImageLayer::CDTImageLayer(const QString &p, const QString &n):
//    path(p),name(n) //for test
//{
//    QVector<CDTSegmentationLayer> s;
//    s.push_back(CDTSegmentationLayer("segment1","c:/","MST"));
//    s.push_back(CDTSegmentationLayer("segment2","c:/","MST"));
//    setsegmentations(s);
//}

void CDTImageLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *imageroot =new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE_ROOT,name);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,QObject::tr("path"));
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,path);
    CDTProjectTreeItem *segmentationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,QObject::tr("segmentations"));

    imageroot->setChild(0,0,param);
    imageroot->setChild(0,1,value);
    imageroot->setChild(1,segmentationsroot);
    parent->appendRow(imageroot);

    for(int i=0;i<segmentations.size();++i)
    {
        segmentations[i].updateTreeModel(segmentationsroot);
    }
}

//void CDTImageLayer::setsegmentations(QVector<CDTSegmentationLayer> &s)
//{
//    for(int i=0;i<s.size();++i)
//    {
//        segmentations.push_back(s[i]);
//    }
//}

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image)
{
    out<<image.path<<image.name<<image.segmentations;
    return out ;
}


QDataStream &operator>>(QDataStream &in, CDTImageLayer &image)
{
    in>>image.path>>image.name>>image.segmentations;
    return in;
}
