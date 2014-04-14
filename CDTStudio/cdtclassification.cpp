#include "cdtclassification.h"
#include "cdtsegmentationlayer.h"
#include <QMenu>

CDTClassification::CDTClassification(QUuid uuid, QObject* parent)
    :CDTBaseObject(uuid,parent),
      actionRemoveClassification(new QAction(tr("Remove Classification"),this))
{
    connect(actionRemoveClassification,SIGNAL(triggered()),this,SLOT(remove()));
    connect(this,SIGNAL(removeClassification(CDTClassification*)),(CDTSegmentationLayer*)(this->parent()),SLOT(removeClassification(CDTClassification*)));
}

void CDTClassification::updateTreeModel(CDTProjectTreeItem *parent)
{
    CDTProjectTreeItem *classification =new CDTProjectTreeItem(
                CDTProjectTreeItem::CLASSIFICATION,CDTProjectTreeItem::GROUP,m_name,this);
    CDTProjectTreeItem *param =new CDTProjectTreeItem(
                CDTProjectTreeItem::PARAM,CDTProjectTreeItem::VECTOR,tr("Shapefile path"),this);
    CDTProjectTreeItem *value =new CDTProjectTreeItem(
                CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,m_shapefilePath,this);
    CDTProjectTreeItem *methodroot =new CDTProjectTreeItem(
                CDTProjectTreeItem::METHOD_PARAMS,CDTProjectTreeItem::EMPTY,tr("Method"),this);
    CDTProjectTreeItem *methodrootvalue =new CDTProjectTreeItem(
                CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,m_method,this);

    for(int i=0;i<m_params.size();++i)
    {
        QList<QString> keys =m_params.keys();
        CDTProjectTreeItem *methodparam =new CDTProjectTreeItem(
                    CDTProjectTreeItem::PARAM,CDTProjectTreeItem::EMPTY,keys[i],this);
        CDTProjectTreeItem *methodvalue =new CDTProjectTreeItem(
                    CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,m_params[keys[i]].toString(),this);
        methodroot->setChild(i,0,methodparam);
        methodroot->setChild(i,1,methodvalue);
    }

    classification->setChild(0,0,param);
    classification->setChild(0,1,value);
    classification->setChild(1,0,methodroot);
    classification->setChild(1,1,methodrootvalue);

    parent->appendRow(classification);
}

void CDTClassification::onContextMenuRequest(QWidget *parent)
{
    actionRemoveClassification->setIcon(QIcon(":/Icon/remove.png"));
    QMenu *menu =new QMenu(parent);

    menu->addAction(actionRemoveClassification);
    menu->exec(QCursor::pos());
}

void CDTClassification::remove()
{
    emit removeClassification(this);
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



