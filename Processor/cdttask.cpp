#include "cdttask.h"

CDTTask::CDTTask(QString id, QDomElement params, QObject *parent) :
    QObject(parent)
{
    this->id = id;
    this->params = params;    
}
