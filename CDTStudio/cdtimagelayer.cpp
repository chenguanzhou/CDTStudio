#include "cdtimagelayer.h"
#include <QMenu>
#include "dialognewsegmentation.h"

CDTImageLayer::CDTImageLayer(QObject *parent)
    :CDTBaseObject(parent),
      addSegmentationLayer(new QAction(tr("Add Segmentation"),this))
{
    //    segmentations.push_back(CDTSegmentationLayer("segment1","c:/","MST"));
    //    segmentations.push_back(CDTSegmentationLayer("segment2","c:/","MST"));
//    QMap<QString,QVariant> params;
//    params["threshold"] = 25;
//    params["minArea"] = 100;

//    CDTSegmentationLayer *segment1 = new CDTSegmentationLayer(this);
//    segment1->setName("seg1");
//    segment1->setShapefilePath("c:/seg1.shp");
//    segment1->setMethodParams("mst",params);

//    CDTSegmentationLayer *segment2 = new CDTSegmentationLayer(this);
//    segment2->setName("seg2");
//    segment2->setShapefilePath("c:/seg1.shp");
//    segment2->setMethodParams("mst",params);

//    addSegmentation(segment1);
//    addSegmentation(segment2);

    connect(addSegmentationLayer,SIGNAL(triggered()),this,SLOT(addSegmentation()));
}

void CDTImageLayer::setPath(const QString &path)
{
    m_path = path;
    emit pathChanged(m_path);
    emit imageLayerChanged();
}

void CDTImageLayer::setName(const QString &name)
{
    m_name = name;
    emit nameChanged(m_name);
    emit imageLayerChanged();
}

void CDTImageLayer::addSegmentation(CDTSegmentationLayer *segmentation)
{
    segmentations.push_back(segmentation);
    emit imageLayerChanged();
    connect(segmentation,SIGNAL(segmentationChanged()),this,SIGNAL(imageLayerChanged()));
}

void CDTImageLayer::addSegmentation()
{
    DialogNewSegmentation* dlg = new DialogNewSegmentation(m_path);
    if(dlg->exec()==DialogNewSegmentation::Accepted)
    {
        CDTSegmentationLayer *segmentation = new CDTSegmentationLayer();
        segmentation->setName(dlg->name());
        segmentation->setShapefilePath(dlg->shapefilePath());
        segmentation->setMarkfilePath(dlg->markfilePath());
        segmentation->setMethodParams(dlg->method(),dlg->params());
        addSegmentation(segmentation);
    }
    delete dlg;
}

void CDTImageLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *imageroot =new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE_ROOT,m_name,this);
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_path,this);
    CDTProjectTreeItem *segmentationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,tr("segmentations"),this);

    parent->setChild(parent->rowCount(),0,imageroot);
    parent->setChild(parent->rowCount()-1,1,value);
    imageroot->setChild(0,segmentationsroot);

    for(int i=0;i<segmentations.size();++i)
    {
        segmentations[i]->updateTreeModel(segmentationsroot);
    }
}

void CDTImageLayer::onContextMenuRequest(QWidget *parent)
{
    QMenu *menu =new QMenu(parent);

    menu->addAction(addSegmentationLayer);
    menu->exec(QCursor::pos());
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
