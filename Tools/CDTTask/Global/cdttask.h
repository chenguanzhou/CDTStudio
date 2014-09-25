#ifndef CDTTASK_H
#define CDTTASK_H

#include <QtCore>

namespace CDTTask {

enum DataType{
    Debug = 0,
    TaskInfo,
    TaskResult
};

class CDTTaskInfo
{
public:
    enum Status{
        WAITING,
        PROCESSING,
        COMPLETED
    };
    CDTTaskInfo()
        :status(WAITING),currentProgress(0),totalProgress(0)
    { }

    friend QDataStream & operator<< (QDataStream& out, const CDTTaskInfo& info);
    friend QDataStream & operator>> (QDataStream& in, CDTTaskInfo& info);

    Status status;
    QString currentStep;
    int currentProgress;
    int totalProgress;
};

//QDataStream & operator<< (QDataStream& out, const CDTTaskInfo& info)
//{
//    out<<info.status<<info.currentStep<<info.currentProgress<<info.totalProgress;
//    return out;
//}
//QDataStream & operator>> (QDataStream& in, CDTTaskInfo& info)
//{
//    int status;
//    in>>status>>info.currentStep>>info.currentProgress>>info.totalProgress;
//    info.status = static_cast<CDTTaskInfo::Status>(status);
//    return in;
//}

}
#endif // CDTTASK_H
