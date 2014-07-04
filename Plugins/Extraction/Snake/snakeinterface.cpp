#include "snakeinterface.h"

SnakeInterface::SnakeInterface(QUuid extravtionLayerID,QObject *parent)
    :CDTExtractionInterface(extravtionLayerID,parent)
{
}

QString SnakeInterface::methodName() const
{
    return tr("Snake");
}

QString SnakeInterface::description() const
{
    return tr("Snake active contour");
}

void SnakeInterface::start()
{

}

void SnakeInterface::rollback()
{

}

void SnakeInterface::save()
{

}

void SnakeInterface::stop()
{

}

