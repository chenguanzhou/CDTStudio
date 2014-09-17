#include "cdttask_pbcdbinary.h"
#include <QtCore>
#include "cdttaskmanager.h"
#include "cdtprocessorapplication.h"


CDTTask_PBCDBinary::CDTTask_PBCDBinary(QString id,QDomElement params, QObject *parent) :
    CDTTask(id,params,parent)
{

}

void CDTTask_PBCDBinary::start()
{
    qApp->returnDebugMessage("start");
    info.status = CDTTaskInfo::PROCESSING;
    info.currentStep = "hehe";
    for(int i=0;i<100;i+=10)
    {
        info.currentProgress = i;
        info.totalProgress = i;
        emit taskInfoUpdated(id,info);
    }

    info.currentProgress = info.totalProgress = 100;
    info.status = CDTTaskInfo::COMPLETED;
    emit taskInfoUpdated(id,info);
}
