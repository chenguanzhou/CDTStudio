#include "cdttask.h"

CDTTask::CDTTask(QString id, QDomDocument params, QObject *parent) :
    QObject(parent)
{
    this->id = id;
    this->params = params;
}

void CDTTask::error(QString errorText)
{
    info.status = CDTTaskInfo::COMPLETED;
    info.currentStep = errorText;
    emit taskInfoUpdated(id,info);
}
