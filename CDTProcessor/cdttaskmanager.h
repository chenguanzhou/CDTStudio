#ifndef CDTTASKMANAGER_H
#define CDTTASKMANAGER_H

#include <QObject>
#include <QMetaObject>
#include <QMap>
#include <QQueue>
#include "cdttask.h"
struct QUuid;
class QDomElement;

class CDTTaskManager : public QObject
{
    Q_OBJECT
public:
    explicit CDTTaskManager(QObject *parent = 0);
    static void registerTask(QString taskName, QMetaObject metaObj);

    bool appendNewTask(QString name, QString id, QDomDocument params);
    void queryTaskInfo(QString id);
signals:
    void taskAppended(QString id);
    void taskInfoUpdated(QString id,CDTTaskInfo info);
    void taskCompleted(QString id,QByteArray result);
public slots:
    void onTaskCompleted(QString id,QByteArray result);
private:
    QMap<QString,CDTTask*> tasks;
    QString currentTaskID;
    QQueue<QString> waitingTasksQueue;
    static QMap<QString,QMetaObject> availableTasks;
};

#endif // CDTTASKMANAGER_H
