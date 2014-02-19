#include "cdtsample.h"
#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include <QList>
#include <QMenu>


CDTSegmentationLayer::CDTSegmentationLayer(QObject *parent)
    :QObject(parent),addClassifications(new QAction(tr("Add Classification"),this))
{
    QMap<QString,QVariant> params;
    params["K"] = 32;
    CDTClassification *classification = new CDTClassification(this);
    classification->setName("cls1");
    classification->setShapefilePath("c:/cls1.shp");
    classification->setMethodParams("knn",params);
    classifications.push_back(classification);

    connect(addClassifications,SIGNAL(triggered()),this,SLOT(addClassification()));
}

void CDTSegmentationLayer::addClassification(CDTClassification *classification)
{
    classifications.push_back(classification);
}

//CDTSegmentationLayer::CDTSegmentationLayer(const QString &n, const QString &s, const QString &m):
//    name(n),shapefilePath(s),method(m)
//{
//    params["threshold"] =25;
//    params["minArea"] =100;

//    classifications.push_back(CDTClassification("cls1","c:/","knn"));
//    classifications.push_back(CDTClassification("cls2","c:/","knn"));

//}

void CDTSegmentationLayer::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *segment =new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION,m_name,this);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,tr("shapefilePath"),NULL);
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_shapefilePath,NULL);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,tr("method"),this);
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_method,NULL);
    CDTProjectTreeItem *classificationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION_ROOT,tr("classifications"),this);

    for(int i=0;i<m_params.size();++i)
    {
        QList<QString> keys =m_params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,keys[i],NULL);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_params[keys[i]].toString(),NULL);
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
        classifications[i]->updateTreeModel(classificationsroot);
    }


}

void CDTSegmentationLayer::onContextMenu()
{
    QMenu *menu =new QMenu;
    menu->addAction(addClassifications);

    menu->exec(QCursor::pos());
}

void CDTSegmentationLayer::addClassification()
{
    QMap<QString,QVariant> param;
    param["k"] ="new k";

    CDTClassification *classification =new CDTClassification();
    classification->setName("new classification");
    classification->setShapefilePath("new shapefilepath");
    classification->setMethodParams("new knn",param);

    classifications.push_back(classification);

}

QString CDTSegmentationLayer::name() const
{
    return m_name;
}

QString CDTSegmentationLayer::shapefilePath() const
{
    return m_shapefilePath;
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

void CDTSegmentationLayer::setMethodParams(const QString &methodName, const QMap<QString, QVariant> &params)
{
    m_method = methodName;
    m_params = params;
}

QDataStream &operator<<(QDataStream &out, const CDTSegmentationLayer &segmentation)
{
    out<<segmentation.m_name<<segmentation.m_shapefilePath<<segmentation.m_method
      <<segmentation.m_params<<segmentation.attributes<<segmentation.samples;

    out<<segmentation.classifications.size();
    for (int i=0;i<segmentation.classifications.size();++i)
        out<<*(segmentation.classifications[i]);

    return out;
}


QDataStream &operator>>(QDataStream &in,CDTSegmentationLayer &segmentation)
{
    in>>segmentation.m_name>>segmentation.m_shapefilePath>>segmentation.m_method
     >>segmentation.m_params>>segmentation.attributes>>segmentation.samples;

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
