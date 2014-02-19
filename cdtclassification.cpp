#include "cdtclassification.h"

CDTClassification::CDTClassification(QObject* parent)
    :QObject(parent)
{

}

void CDTClassification::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *classification =new CDTProjectTreeItem(CDTProjectTreeItem::CLASSIFICATION,m_name,NULL);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,tr("shapefilePath"),NULL);
    CDTProjectTreeItem *value =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_shapefilePath,NULL);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(CDTProjectTreeItem::METHOD_PARAMS,tr("method"),NULL);
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_method,NULL);

    for(int i=0;i<m_params.size();++i)
    {
        QList<QString> keys =m_params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,keys[i],NULL);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,m_params[keys[i]].toString(),NULL);
        methodroot->setChild(i,0,methodparam);
        methodroot->setChild(i,1,methodvalue);
    }

    classification->setChild(0,0,param);
    classification->setChild(0,1,value);
    classification->setChild(1,0,methodroot);
    classification->setChild(1,1,methodrootvalue);

    parent->appendRow(classification);
}

QString CDTClassification::name() const
{
    return m_name;
}

QString CDTClassification::shapefilePath() const
{
    return m_shapefilePath;
}

QString CDTClassification::method() const
{
    return m_method;
}

void CDTClassification::setName(const QString &name)
{
    m_name = name;
    emit nameChanged();
}

void CDTClassification::setShapefilePath(const QString &shpPath)
{
    m_shapefilePath = shpPath;
    emit shapefilePathChanged();
}

void CDTClassification::setMethodParams(const QString &methodName, const QMap<QString, QVariant> &params)
{
    m_method = methodName;
    m_params = params;
}

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification)
{
    out<<classification.m_name<<classification.m_shapefilePath
      <<classification.m_method<<classification.m_params;
    return out;
}


QDataStream &operator>>(QDataStream &in, CDTClassification &classification)
{
    in>>classification.m_name>>classification.m_shapefilePath
      >>classification.m_method>>classification.m_params;
    return in;
}



