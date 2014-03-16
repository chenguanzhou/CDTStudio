#include "cdtimagelayer.h"
#include <QMenu>
#include "dialognewsegmentation.h"
#include "cdtproject.h"

CDTImageLayer::CDTImageLayer(QObject *parent)
    :CDTBaseObject(parent),
      addSegmentationLayer(new QAction(tr("Add Segmentation"),this)),
      removeImage(new QAction(tr("Remove Image"),this)),
      removeAllSegmentations(new QAction(tr("Remove All Segmentations"),this))
{
    connect(addSegmentationLayer,SIGNAL(triggered()),this,SLOT(addSegmentation()));
    connect(removeImage,SIGNAL(triggered()),this,SLOT(remove()));
    connect(this,SIGNAL(removeImageLayer(CDTImageLayer*)),(CDTProject*)(this->parent()),SLOT(removeImageLayer(CDTImageLayer*)));
    connect(removeAllSegmentations,SIGNAL(triggered()),this,SLOT(removeAllSegmentationLayers()));
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
}

void CDTImageLayer::addSegmentation()
{
    DialogNewSegmentation* dlg = new DialogNewSegmentation(m_path);
    if(dlg->exec()==DialogNewSegmentation::Accepted)
    {
        CDTSegmentationLayer *segmentation = new CDTSegmentationLayer(m_path,this);
        segmentation->setName(dlg->name());
        segmentation->setShapefilePath(dlg->shapefilePath());
        segmentation->setMarkfilePath(dlg->markfilePath());
        segmentation->setMethodParams(dlg->method(),dlg->params());
        addSegmentation(segmentation);
    }
    delete dlg;
}

void CDTImageLayer::remove()
{
    emit removeImageLayer(this);
}

void CDTImageLayer::removeSegmentation(CDTSegmentationLayer *sgmt)
{
    for(int i =0;i <segmentations.size();++i)
    {
        if(sgmt->name() == segmentations[i]->name())
        {
            segmentations.remove(i);
            emit imageLayerChanged();
        }
    }    
}

void CDTImageLayer::removeAllSegmentationLayers()
{
    segmentations.clear();
    emit imageLayerChanged();
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

    removeImage->setIcon(QIcon(":/Icon/remove.png"));
    removeAllSegmentations->setIcon(QIcon(":/Icon/remove.png"));
    addSegmentationLayer->setIcon(QIcon(":/Icon/add.png"));
    QMenu *menu =new QMenu(parent);

    menu->addAction(addSegmentationLayer);
    menu->addSeparator();
    menu->addAction(removeImage);
    menu->addAction(removeAllSegmentations);

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
        CDTSegmentationLayer* segmentation = new CDTSegmentationLayer(image.m_path,&image);
        in>>(*segmentation);
        image.segmentations.push_back(segmentation);
    }
    return in;
}
