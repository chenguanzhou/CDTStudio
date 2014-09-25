#ifndef CDTTASK_H
#define CDTTASK_H

#include <QObject>
#include <QDomElement>

class CDTTaskInfo
{
public:
    enum Status{
        WAITING,
        PROCESSING,
        COMPLETED
    };
    CDTTaskInfo():status(WAITING),currentProgress(0),totalProgress(0){}

    Status status;
    QString currentStep;
    int currentProgress;
    int totalProgress;
};

class CDTTask : public QObject
{
    Q_OBJECT
public:
    explicit CDTTask(QString id, QDomDocument params, QObject *parent = 0);

signals:
    void taskInfoUpdated(QString id,CDTTaskInfo taskInfo);
    void taskCompleted(QString id,QByteArray result);
public slots:
    virtual void start() =0;
    void error(QString errorText);

protected:
    QString id;
    QString name;
    QDomDocument params;
    CDTTaskInfo info;
};

#endif // CDTTASK_H
