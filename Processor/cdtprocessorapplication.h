 #ifndef CDTPROCESSORAPPLICATION_H
#define CDTPROCESSORAPPLICATION_H

#include <QCoreApplication>
#include "cdttask.h"

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<CDTProcessorApplication *>(QCoreApplication::instance()))

class QUdpSocket;
class CDTTaskManager;
class CDTProcessorApplication : public QCoreApplication
{
    Q_OBJECT
public:
    explicit CDTProcessorApplication(int &argc, char **argv);

    static QString getTempFileName(QString suffix);

private:
    void initPlugins();

signals:

public slots:
    void readCommand();
    void parseCommand(QByteArray data);
    void returnDebugMessage(QString msg);
    void returnTaskInfo(QString id, CDTTaskInfo info);
    void returnTaskResult(QString id,QByteArray data);

    void onTaskAppended(QString id);
    void onTaskInfoUpdated(QString id,CDTTaskInfo info);
    void onTaskCompleted(QString id,QByteArray result);

private:
    qint16 portUpload;
    qint16 portDownload;
    QUdpSocket *udpReceiver;
    QUdpSocket *udpSender;

    CDTTaskManager *taskManager;
};

#endif // CDTPROCESSORAPPLICATION_H
