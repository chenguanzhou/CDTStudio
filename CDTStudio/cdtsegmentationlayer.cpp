#include "cdtsample.h"
#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include "cdtimagelayer.h"
#include <QList>
#include <QMenu>


CDTSegmentationLayer::CDTSegmentationLayer(QObject *parent)
    :CDTBaseObject(parent),
      addClassifications(new QAction(tr("Add Classification"),this)),
      actionRemoveSegmentation(new QAction(tr("Remove Segmentation"),this)),
      actionRemoveAllClassifications(new QAction(tr("Remove All Classifications"),this))
{
    connect(addClassifications,SIGNAL(triggered()),this,SLOT(addClassification()));

    connect(this,SIGNAL(markfilePathChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(nameChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(shapefilePathChanged()),this,SIGNAL(segmentationChanged()));
    connect(this,SIGNAL(methodParamsChanged()),this,SIGNAL(segmentationChanged()));

    connect(actionRemoveSegmentation,SIGNAL(triggered()),this,SLOT(remove()));
    connect(this,SIGNAL(removeSegmentation(CDTSegmentationLayer*)),(CDTImageLayer*)(this->parent()),SLOT(removeSegmentation(CDTSegmentationLayer*)));
    connect(this,SIGNAL(segmentationChanged()),(CDTImageLayer*)(this->parent()),SIGNAL(imageLayerChanged()));

    connect(actionRemoveAllClassifications,SIGNAL(triggered()),this,SLOT(removeAllClassifications()));
}

void CDTSegmentationLayer::addClassification(CDTClassification *classification)
{
    classifications.push_back(classification);
    emit methodParamsChanged();
    connect(classification,SIGNAL(methodParamsChanged()),this,SIGNAL(segmentationChanged()));
}

void CDTSegmentationLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *segment =new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION,m_name,this);
    CDTProjectTreeItem *paramShp =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,tr("Shapefile path"),this);
    CDTProjectTreeItem *valueShp =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_shapefilePath,this);
    CDTProjectTreeItem *paramMk =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,tr("Markfile path"),this);
    CDTProjectTreeItem *valueMk =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_markfilePath,this);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,tr("Method"),this);
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_method,this);
    CDTProjectTreeItem *classificationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION_ROOT,tr("Classifications"),this);

    for(int i=0;i<m_params.size();++i)
    {
        QList<QString> keys =m_params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,keys[i],this);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_params[keys[i]].toString(),this);
        methodroot->setChild(i,0,methodparam);
        methodroot->setChild(i,1,methodvalue);
    }

    segment->setChild(0,0,paramShp);
    segment->setChild(0,1,valueShp);
    segment->setChild(1,0,paramMk);
    segment->setChild(1,1,valueMk);
    segment->setChild(2,0,methodroot);
    segment->setChild(2,1,methodrootvalue);
    segment->setChild(3,classificationsroot);

    parent->appendRow(segment);

    for(int i=0;i<classifications.size();++i)
    {
        classifications[i]->updateTreeModel(classificationsroot);
    }
}

void CDTSegmentationLayer::onContextMenuRequest(QWidget *parent)
{
    actionRemoveSegmentation->setIcon(QIcon(":/Icon/remove.png"));
    actionRemoveAllClassifications->setIcon(QIcon(":/Icon/remove.png"));
    addClassifications->setIcon(QIcon(":/Icon/add.png"));
    QMenu *menu =new QMenu;
    menu->addAction(addClassifications);
    menu->addSeparator();
    menu->addAction(actionRemoveSegmentation);
    menu->addAction(actionRemoveAllClassifications);
    menu->exec(QCursor::pos());
}

void CDTSegmentationLayer::addClassification()
{
    QMap<QString,QVariant> param;
    param["k"] ="new k";

    CDTClassification *classification =new CDTClassification(this);
    classification->setName("new classification");
    classification->setShapefilePath("new shapefilepath");
    classification->setMethodParams("new knn",param);

    addClassification(classification);
    qDebug();
}

void CDTSegmentationLayer::remove()
{
    emit removeSegmentation(this);
}

void CDTSegmentationLayer::removeClassification(CDTClassification* clf)
{
    for(int i =0;i <classifications.size();++i)
    {
        if(clf->name() == classifications[i]->name())
        {
            classifications.remove(i);
            emit segmentationChanged();
            connect(clf,SIGNAL(methodParamsChanged()),this,SIGNAL(segmentationChanged()));

        }
    }
}

void CDTSegmentationLayer::removeAllClassifications()
{
    classifications.clear();
    emit segmentationChanged();
}

QString CDTSegmentationLayer::name() const
{
    return m_name;
}

QString CDTSegmentationLayer::shapefilePath() const
{
    return m_shapefilePath;
}

QString CDTSegmentationLayer::markfilePath() const
{
    return m_markfilePath;
}

QString CDTSegmentationLayer::method() const
{
    return m_method;
}

void CDTSegmentationLayer::setName(const QString &name)
{
    m_name = name;
    emit nameChanged();    
}

void CDTSegmentationLayer::setShapefilePath(const QString &shpPath)
{
    m_shapefilePath = shpPath;
    emit shapefilePathChanged();
}

void CDTSegmentationLayer::setMarkfilePath(const QString &mkPath)
{
    m_markfilePath = mkPath;
    emit markfilePathChanged();
}

void CDTSegmentationLayer::setMethodParams(const QString &methodName, const QMap<QString, QVariant> &params)
{
    m_method = methodName;
    m_params = params;
}

QDataStream &operator<<(QDataStream &out, const CDTSegmentationLayer &segmentation)
{
    out<<segmentation.m_name<<segmentation.m_shapefilePath<<segmentation.m_method
      <<segmentation.m_params<<segmentation.attributes<<segmentation.samples<<segmentation.m_markfilePath;

    out<<segmentation.classifications.size();
    for (int i=0;i<segmentation.classifications.size();++i)
        out<<*(segmentation.classifications[i]);

    return out;
}


QDataStream &operator>>(QDataStream &in,CDTSegmentationLayer &segmentation)
{
    in>>segmentation.m_name>>segmentation.m_shapefilePath>>segmentation.m_method
     >>segmentation.m_params>>segmentation.attributes>>segmentation.samples>>segmentation.m_markfilePath;

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTClassification* classification = new CDTClassification(&segmentation);
        in>>*classification;
        segmentation.classifications.push_back(classification);
    }
    return in;
}
