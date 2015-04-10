#include "cdtsegmentationhelper.h"
#include "cdtsegmentationinterface.h"

CDTSegmentationHelper::CDTSegmentationHelper(CDTSegmentationInterface *plugin,QObject *parent)
    :CDTBaseThread(parent)
{
    this->plugin = plugin;
}

CDTSegmentationHelper::~CDTSegmentationHelper()
{

}

bool CDTSegmentationHelper::isValid() const
{
    return plugin!=NULL;
}

void CDTSegmentationHelper::setMarkfilePath(const QString &path)
{
    markfilePath=path;
}

void CDTSegmentationHelper::setShapefilePath(const QString &path)
{
    shapefilePath=path;
}

void CDTSegmentationHelper::run()
{
    if (plugin)
    {
        connect(plugin,SIGNAL(currentProgressChanged(QString)),this,SIGNAL(currentProgressChanged(QString)));
        connect(plugin,SIGNAL(progressBarSizeChanged(int,int)),this,SIGNAL(progressBarSizeChanged(int,int)));
        connect(plugin,SIGNAL(progressBarValueChanged(int)),this,SIGNAL(progressBarValueChanged(int)));
        connect(plugin,SIGNAL(finished()),this,SIGNAL(finished()));

        plugin->setMarkfilePath(markfilePath);
        plugin->setShapefilePath(shapefilePath);
        plugin->startSegmentation();
    }
}

