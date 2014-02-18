#include "cdtimagelayer.h"

CDTImageLayer::CDTImageLayer(QObject *parent)
    :QObject(parent)
{
//    segmentations.push_back(CDTSegmentationLayer("segment1","c:/","MST"));
//    segmentations.push_back(CDTSegmentationLayer("segment2","c:/","MST"));
    QMap<QString,QVariant> params;
    params["threshold"] = 25;
    params["minArea"] = 100;

    CDTSegmentationLayer *segment1 = new CDTSegmentationLayer(this);
    segment1->setName("seg1");
    segment1->setShapefilePath("c:/seg1.shp");
    segment1->setMethodParams("mst",params);

    CDTSegmentationLayer *segment2 = new CDTSegmentationLayer(this);
    segment2->setName("seg2");
    segment2->setShapefilePath("c:/seg1.shp");
    segment2->setMethodParams("mst",params);

    addSegmentation(segment1);
    addSegmentation(segment2);
}

void CDTImageLayer::setPath(const QString &path)
{
    m_path = path;
    emit pathChanged(m_path);
}

void CDTImageLayer::setName(const QString &name)
{
    m_name = name;
    emit nameChanged(m_name);
}

void CDTImageLayer::addSegmentation(CDTSegmentationLayer *segmentation)
{
    segmentations.push_back(segmentation);
}

void CDTImageLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *imageroot =new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE_ROOT,m_name);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,QObject::tr("path"));
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_path);
    CDTProjectTreeItem *segmentationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,QObject::tr("segmentations"));

    imageroot->setChild(0,0,param);
    imageroot->setChild(0,1,value);
    imageroot->setChild(1,segmentationsroot);
    parent->appendRow(imageroot);

    for(int i=0;i<segmentations.size();++i)
    {
        segmentations[i]->updateTreeModel(segmentationsroot);
    }
}

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image)
{
    out<<image.m_path<<image.m_name<<image.segmentations.size();
    for (int i=0;i<image.segmentations.size();++i)
        out<<*(image.segmentations[i]);
    return out ;
}


QDataStream &operator>>(QDataStream &in, CDTImageLayer &image)
{
    in>>image.m_path>>image.m_name;
    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTSegmentationLayer* segmentation = new CDTSegmentationLayer(&image);
        in>>(*segmentation);
        image.segmentations.push_back(segmentation);
    }
    return in;
}
