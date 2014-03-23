#include "cdtimagelayer.h"
#include <QMenu>
#include <QInputDialog>
#include "dialognewsegmentation.h"
#include "cdtproject.h"
#include <qgsrasterlayer.h>
#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
#include <QMessageBox>

CDTImageLayer::CDTImageLayer(QObject *parent)
    :CDTBaseObject(parent),
      addSegmentationLayer(new QAction(tr("Add Segmentation"),this)),
      removeImage(new QAction(tr("Remove Image"),this)),
      removeAllSegmentations(new QAction(tr("Remove All Segmentations"),this)),
      actionRename(new QAction(tr("Rename Image"),this))
{
    keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::RASTER,QString(),this);
    valueItem=new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    segmentationsroot = new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,CDTProjectTreeItem::GROUP,tr("segmentations"),this);
    keyItem->appendRow(segmentationsroot);

    connect(addSegmentationLayer,SIGNAL(triggered()),this,SLOT(addSegmentation()));
    connect(removeImage,SIGNAL(triggered()),this,SLOT(remove()));
    connect(this,SIGNAL(removeImageLayer(CDTImageLayer*)),(CDTProject*)(this->parent()),SLOT(removeImageLayer(CDTImageLayer*)));
    connect(removeAllSegmentations,SIGNAL(triggered()),this,SLOT(removeAllSegmentationLayers()));
    connect(actionRename,SIGNAL(triggered()),this,SLOT(onActionRename()));    
}

void CDTImageLayer::setPath(const QString &path)
{
    qDebug()<<"old:"<<m_path<<"\tnew:"<<path;
    if (m_path == path)
        return;

    m_path = path;
    valueItem->setText(m_path);
    if (mapCanvasLayer)
        delete mapCanvasLayer;
    mapCanvasLayer = new QgsRasterLayer(path,QFileInfo(path).completeBaseName());
    if (!mapCanvasLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open image ")+path+tr(" failed!"));
        delete mapCanvasLayer;
    }
    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer,TRUE);
    keyItem->setMapLayer(mapCanvasLayer);
    emit pathChanged(m_path);
    emit appendLayer(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit imageLayerChanged();
}

void CDTImageLayer::setName(const QString &name)
{
    m_name = name;
    keyItem->setText(m_name);
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
        segmentationsroot->appendRow(segmentation->standardItems());
        addSegmentation(segmentation);
    }
    delete dlg;
}

void CDTImageLayer::remove()
{
    emit removeLayer(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit removeImageLayer(this);    
}

void CDTImageLayer::removeSegmentation(CDTSegmentationLayer *sgmt)
{
    int index = segmentations.indexOf(sgmt);
    if (index>=0)
    {
        QStandardItem* keyItem = sgmt->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        segmentations.remove(index);
        delete sgmt;
        emit imageLayerChanged();
    }
}

void CDTImageLayer::removeAllSegmentationLayers()
{
    foreach (CDTSegmentationLayer* sgmt, segmentations) {
        QStandardItem* keyItem = sgmt->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        delete sgmt;
    }
    segmentations.clear();
    emit imageLayerChanged();
}

void CDTImageLayer::onActionRename()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Input Image Name"),
                                         tr("Image name:"), QLineEdit::Normal,
                                         m_name, &ok);
    if (ok && !text.isEmpty())
    {
        setName(text);

    }
}


void CDTImageLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *imageroot =new CDTProjectTreeItem(
                CDTProjectTreeItem::IMAGE_ROOT,CDTProjectTreeItem::RASTER,m_name,this);
    CDTProjectTreeItem *value =new CDTProjectTreeItem(
                CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,m_path,this);
    CDTProjectTreeItem *segmentationsroot =new CDTProjectTreeItem(
                CDTProjectTreeItem::SEGMENTION_ROOT,CDTProjectTreeItem::GROUP,tr("segmentations"),this);

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
    actionRename->setIcon(QIcon(":/Icon/rename.png"));
    QMenu *menu =new QMenu(parent);

    menu->addAction(addSegmentationLayer);
    menu->addAction(removeImage);
    menu->addAction(removeAllSegmentations);
    menu->addSeparator();
    menu->addAction(actionRename);

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
    QString temp;
    in>>temp;
    image.setPath(temp);
    in>>temp;
    image.setName(temp);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTSegmentationLayer* segmentation = new CDTSegmentationLayer(image.m_path,&image);
        in>>(*segmentation);
        image.segmentationsroot->appendRow(segmentation->standardItems());
        image.segmentations.push_back(segmentation);
    }
    return in;
}
