#include "cdtsample.h"
#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include <QList>


CDTSegmentationLayer::CDTSegmentationLayer(QObject *parent)
    :QObject(parent)
{
    QMap<QString,QVariant> params;
    params["K"] = 32;
    CDTClassification *classification = new CDTClassification(this);
    classification->setName("cls1");
    classification->setShapefilePath("c:/cls1.shp");
    classification->setMethodParams("knn",params);
    classifications.push_back(classification);
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
    CDTProjectTreeItem *segment =new CDTProjectTreeItem(CDTProjectTreeItem::SEGMENTION_ROOT,m_name);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,tr("shapefilePath"));
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_shapefilePath);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,tr("method"));
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_method);
    CDTProjectTreeItem *classificationsroot =new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION_ROOT,tr("classifications"));

    for(int i=0;i<m_params.size();++i)
    {
        QList<QString> keys =m_params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,keys[i]);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_params[keys[i]].toString());
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
